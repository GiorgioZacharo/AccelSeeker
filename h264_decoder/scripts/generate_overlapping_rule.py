with open('FCI_CROPPED.txt', 'r') as file:
    content = file.read()

content = content.split('\n')

new_file = ""
for c in content:
    c_split = c.split(" ")
    if len(c_split) < 2:
	line = c
    	new_file += line + '\n'
        continue
    name = c_split[0]
    p = c_split[1]
    search = c_split[2:]
    tmp2 = []

    for s in search:
        for c2 in content:
            c2_split = c2.split(" ")
            if len(c2_split) < 2:
                continue
            pivot = c2_split[1]
            if s == pivot:
                tmp2.append(s)
                break
    line = name + " " + p + " " + ' '.join(tmp2)
    new_file += line + '\n'

with open('FCI_CROPPED_OVERLAP_RULE.txt', 'w') as file:
    file.write(new_file)

print new_file

