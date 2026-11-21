from json import load as jload
from textwrap import wrap
from sys import argv

# sl_log_entry_t.flags: bit 0 = formatted string; bits 1+ carry log level (3 bits used, see sl_logging.h).
_SL_FLAGS_LEVEL_SHIFT = 1
_SL_FLAGS_LEVEL_MASK = 0x7

_SL_LEVEL_NAMES = {
    1: "LOG_TRACE",
    2: "LOG_INFO",
    3: "LOG_WARN",
    4: "LOG_ERROR",
    5: "LOG_CRASH",
    6: "LOG_OFF",
}


def _manifest_format_body(fmt):
    """
    Generator writes manifest format as 'COMPONENT: printf_part' (see advanced_logs_generator.py).
    Return the printf substring; if no ': ' separator, treat whole string as the body.
    """
    if not fmt:
        return ""
    sep = fmt.find(": ")
    if sep == -1:
        return fmt
    return fmt[sep + 2 :]


def _iter_format_specs(s):
    """Yield (start_index, token) for each printf conversion in s; skip '%%'."""
    i = 0
    n = len(s)
    while i < n:
        p = s.find("%", i)
        if p < 0:
            return
        if p + 1 < n and s[p + 1] == "%":
            i = p + 2
            continue
        q = p + 1
        while q < n and s[q].isdigit():
            q += 1
        if q >= n:
            return
        q += 1
        yield p, s[p:q]
        i = q


def _count_format_args(fmt):
    return sum(1 for _ in _iter_format_specs(_manifest_format_body(fmt)))


def _next_spec_token(parsee):
    """
    Return the next printf token starting at the first '%' in parsee, or None.
    Skips '%%'. Caller should run this again after each replace() since indices move.
    """
    idx = 0
    n = len(parsee)
    while idx < n:
        p = parsee.find("%", idx)
        if p < 0:
            return None
        if p + 1 < n and parsee[p + 1] == "%":
            idx = p + 2
            continue
        q = p + 1
        while q < n and parsee[q].isdigit():
            q += 1
        if q >= n:
            return None
        q += 1
        return parsee[p:q]
    return None


def _level_string_from_flags(flags, manifest_level):
    """Prefer explicit manifest level; else decode level bits from the log packet."""
    if manifest_level is not None:
        return str(manifest_level)
    wl = (flags >> _SL_FLAGS_LEVEL_SHIFT) & _SL_FLAGS_LEVEL_MASK
    return _SL_LEVEL_NAMES.get(wl, f"LVL_{wl}")


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

        return self.json.get("SL_LOG_VERSION", "unknown")

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
        version = self.get(VERSION_SIZE)

        if did not in self.json:
            raise KeyError("DebugID missing in JSON.")
    
        info = self.json[str(did)]
        if not isinstance(info, dict):
            raise TypeError(f"Manifest entry for {did} must be an object, got {type(info).__name__}")
        parsee = info.get("format", "")
        num_args = info.get("argnum")
        if num_args is None:
            num_args = _count_format_args(parsee)
        else:
            num_args = int(num_args)

        data_left = NUM_ARGS * ARG_SIZE  # bits
        if num_lost_msgs :
            print(f"Number of lost messages: {num_lost_msgs}")

        for argpos in range(num_args):
            sub = _next_spec_token(parsee)
            if sub is None:
                break

            # dropping strings (%5s, %s, ...)
            if sub.startswith("%") and sub.endswith("s"):
                continue

            data_left -= ARG_SIZE
            # data = self.get(ARG_SIZE)
            data = arg_list[argpos]
            if sub in ("%x", "%1x", "%2x", "%4x", "%w") or (
                len(sub) > 2 and sub.endswith("x") and sub.startswith("%")
            ):
                parsee = parsee.replace(sub, hex(data), 1)
            elif sub in ("%d", "%b") or (
                len(sub) > 2 and (sub.endswith("d") or sub.endswith("b")) and sub.startswith("%")
            ):
                parsee = parsee.replace(sub, str(data), 1)
            elif sub in ("%c",) or (len(sub) > 2 and sub.endswith("c") and sub.startswith("%")):
                parsee = parsee.replace(sub, chr(data & 0xFF), 1)
            
        # self.get(data_left)

        level_str = _level_string_from_flags(flags, info.get("level"))
        debug_name = info.get("debug_id", str(did))
        return f"{self.cum_tsf} {level_str} {('HOST', 'NWP')[core_id]} {arg_count & 3} {debug_name} ({parsee})"


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
