from json import load as jload
from textwrap import wrap
from sys import argv

# for testing only
def tostrbits(data):
    ret = ""
    for byte in data:
        #print(byte)
        ret += bin(byte)[2:].zfill(8)
        # ret += bin(int.from_bytes(byte, 'big'))[2:]
    return ret


def b2i(byte):
    return int.from_bytes(byte, "little")


class Decoder:
    """Decoder class"""

    def __init__(self):
        # head indicates current position of state machine
        self.head = 0
        # cumulative tsf
        self.cum_tsf = 0
        # debug data
        self.data = []
        # length of debug data
        self.length = 0

    def _hex_to_byte(self, s):
        """Convert one byte of hex(ASCII) to bytes format"""
        return bytes([int(s, 16)])

    def _int_to_byte(self, x):
        """Convert one byte of int to bytes format"""
        return bytes([x])

    def load_file_json(self, path):
        """Load a JSON file (manifest)"""
        with open(path) as file:
            self.json = jload(file)

    def load_file_bin(self, path):
        """Load binary debug data"""
        with open(path, "rb") as file:
            self.data.extend(list(map(self._int_to_byte, file.read())))
            self.length = 8 * len(self.data)
        print(self.data)

    def load_file_hex(self, path):
        """Load hexadecimal debug data"""
        with open(path, "r") as file:
            self.data.extend(list(map(self._hex_to_byte, file.read().split())))
            self.length = 8 * len(self.data)

    def load_file_srec(self, path):
        """Load SREC debug data"""
        data = []
        with open(path) as file:
            file.readline()
            for line in file.readlines():
                line = wrap(line[9:-3], 2)
                data.extend(line)
        self.data.extend(list(map(self._hex_to_byte, data)))
        self.length = 8 * len(self.data)

    def load_file_ascii(self, path):
        """Load possibly polluted ASCII debug data"""
        data = []
        with open(path) as file:
            for line in file.readlines():
                spline = line.split()
                if spline and spline[0] == "&^$":
                    if spline[-1][0] == '<':
                        spline.pop()
                    data.extend(spline[1:])
        self.data.extend(list(map(self._hex_to_byte, data)))
        self.length = 8 * len(self.data)

    def get(self, n):
        """Get the next n bits from self.data"""

        h = self.head
        bh = h // 8
        ret = 0
        curh = 0
        if h + n > self.length:
            raise ValueError("get value too large.")
        copy_n = n
        while n:
            bufl = 8 - (h % 8)
            chunk = int.from_bytes(self.data[bh], "little") >> (8 - bufl)
            if n < bufl:
                chunk &= ((1 << n) - 1)
                ret = (chunk << curh) | ret
                curh = n = 0
            else:
                ret = (chunk << curh) | ret
                n -= bufl
                curh += bufl
            bh += 1
            h = 0
        self.head += copy_n
        return ret

    def _print_remaining(self):
        """
        Prints remaining contents of self.data
        For debugging only
        """
        for byte in self.data[self.head // 8 :]:
            print(hex(b2i(byte))[2:].zfill(2), end=" ")
        print()

    def get_log_version(self):
        """Get the log version of manifest file"""

        return self.json["SL_LOG_VERSION"]

    def data_to_be_read(self):
        """Return true if there is data to be read"""
        return self.head < self.length

    def read_next(self):
        """Read and parse the next "packet" of data"""
        TSF_SIZE = 32
        LOG_LEVEL_SIZE = 3
        CORE_ID_SIZE = 8
        NUM_ARGS = 3
        EVENT_ID_SIZE = 32
        ARG_COUNT_SIZE = 8
        FLAGS_SIZE = 8
        VERSION_SIZE = 8
        ARG_SIZE = 32

        self.cum_tsf = self.get(TSF_SIZE)
        # self._print_remaining()
        did = str(self.get(EVENT_ID_SIZE))
        did = str(did)
        arg_list = []
        arg_list.append(self.get(ARG_SIZE))
        arg_list.append(self.get(ARG_SIZE))
        arg_list.append(self.get(ARG_SIZE))
        arg_count = self.get(ARG_COUNT_SIZE)
        num_lost_msgs = arg_count >> 2
        core_id = self.get(CORE_ID_SIZE)
        flags = self.get(FLAGS_SIZE)
        log_level = flags >> 1
        version = self.get(VERSION_SIZE)

        if did not in self.json:
            raise KeyError("DebugID missing in JSON.")
    
        info = self.json[str(did)]
        parsee = info["format"]
        num_args = info["argnum"]
        data_left = NUM_ARGS * ARG_SIZE  # bits
        if num_lost_msgs :
            print(f"Number of lost messages: {num_lost_msgs}")
        for argpos in range(num_args):
            idx = parsee.find("%")
            sub = parsee[idx : idx + 2]
            if sub[-1].isnumeric():
                sub += parsee[idx + 2]

            argtype = int(info["arg" + str(argpos + 1) + "type"])

            # dropping strings
            if sub == "%s":
                continue

            data_left -= ARG_SIZE
            # data = self.get(ARG_SIZE)
            data = arg_list[argpos]
            if sub in ("%x", "%1x", "%2x", "%4x", "%w"):
                parsee = parsee.replace(sub, hex(data), 1)
            elif sub in ("%d", "%b"):
                parsee = parsee.replace(sub, str(data), 1)
            elif sub in ("%c"):
                parsee = parsee.replace(sub, chr(data), 1)
            
        # self.get(data_left)

        return f'{self.cum_tsf} {info["level"]} {("HOST", "NWP")[core_id]} {arg_count & 3} {info["debug_id"]} ({parsee})'


def main():

    argc = len(argv)
    if argc < 2:
        print("Usage: python log_decoder.py path/to/dbgdata.txt")
        exit(0)
    decoder = Decoder()
    decoder.load_file_json("manifest.json")
    for fileno in range(1, argc):
        decoder.load_file_ascii(argv[fileno])
    print("SL_LOG_VERSION:", decoder.get_log_version())
    while decoder.data_to_be_read():
        print(decoder.read_next())


if __name__ == "__main__":
    main()
