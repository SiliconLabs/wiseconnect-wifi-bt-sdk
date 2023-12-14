try:	
	import yaml
	from jinja2 import Environment, FileSystemLoader, Template
	import argparse
	import sys 
	import json
	import xmltodict #install xmltodict using pip install xmltodict
except ModuleNotFoundError as e:
	print("Package import failed :", e)
	sys.exit()

def raise_helper(msg):
	raise Exception(msg)
if __name__=="__main__":
	parser = argparse.ArgumentParser(description = "Takes xml/yaml file as input produces output against jinja2 template")
	file_group = parser.add_mutually_exclusive_group()
	file_group.add_argument('-x', help='xml_file')
	file_group.add_argument('-y', help='yaml_file')
	parser.add_argument('-t', help='Jinja Template file')
	parser.add_argument('-o', help='output file')
	args = parser.parse_args()
	try :
		path = (args.t).split("/")
		template_file = path[len(path)-1]
		path = "/".join((path[0:len(path)-1]))
		file_loader = FileSystemLoader(path)
		env = Environment(loader=file_loader, lstrip_blocks=True, trim_blocks=True)
		env.globals['raise'] = raise_helper
	except:
		print("No template file specified")
		sys.exit()
	if(args.y != None):
		try:
			with open(args.y, 'r') as file:
				yaml_output = yaml.safe_load(file)
		except:
			print("File not found : ", args.y)
			sys.exit()
		else:
			template = env.get_template(template_file)
			output = template.render(yaml_output)
			try:
				with open(args.o, 'w') as file:
					file.write(output)
			except:
				print("Output file not specified")
	elif(args.x != None):
		try:
			with open(args.x, 'r') as file:
				xml_file = file.read()
			xml_file = json.loads(json.dumps(xmltodict.parse(xml_file)))
			file_loader = FileSystemLoader('.')
			env = Environment(loader=file_loader, lstrip_blocks=True, trim_blocks=True)
		except:
			print("File not found : ", args.x)
			sys.exit()
		else:
			try:
				template = env.get_template(args.t)
			except:
				print("Template not found")
				sys.exit()
			else:
				output = template.render(xml_file)
				try:
					with open(args.o, 'w') as file:
						file.write(output)
				except:
					print("Output file not specified")
	else:
		print("No args passed")
		sys.exit()