import csv

with open("8086_table.txt") as table_file:
	groups = {}
	for line in csv.reader(table_file, dialect="excel-tab"):
		if (len(line) > 1):
			if line[0].startswith("GRP"):
				group: str = line[0][:-2]
				if group not in groups:
					groups[group] = [('\"' + line[1] + '\"')]
				else:
					groups[group].append('\"' + line[1] + '\"')
			else:
				print(f'{{ 0x{line[0]}, "{line[1]}" }},')
	for group in groups:
		print("string[8]", group, "= {", ', '.join(groups[group]), "};")
	print(groups)