import json
all_str = ''
with open('Countries/countries.geojson', 'r') as f:
    all_str = str(f.read())

dic = json.loads(all_str)

for cur in dic['features']:
    country_name = cur['properties']['ADMIN']
    ls = cur['geometry']['coordinates'][0]
    if cur['geometry']['type'] == 'MultiPolygon':
        ls = cur['geometry']['coordinates'][0][0]

    to_write = []
    to_write.append(str(len(ls)) + '\n')
    for x, y in ls:
        to_write.append(str(x) + ' ' + str(y) + '\n')

    with open(f'Countries/{country_name}.txt', 'w') as f:
        f.writelines(to_write)
