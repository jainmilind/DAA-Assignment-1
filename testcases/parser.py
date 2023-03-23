import json
## all_str hold the whole string of countries.
all_str = ''
with open('Countries/countries.geo.json', 'r') as f:
    all_str = str(f.read())
## dic is a dictionary that stores all the countries and the details regarding each.
dic = json.loads(all_str)

for cur in dic['features']:
    ## country_name is the name of the country
    country_name = cur['properties']['name']
    ## ls is current list of coordinates of the country which can be accessed by country.country_name.
    ls = cur['geometry']['coordinates'][0]
    if cur['geometry']['type'] == 'MultiPolygon':
        ls = cur['geometry']['coordinates']
        ## best is list with maximum number of coordinates in multipolygon countries
        best = list()
        for i in range(len(ls)):
            if len(ls[i][0]) > len(best):
                best = ls[i][0]
        ls = best

    ## to_write is list used to write all the necessary details for  the countries.
    to_write = []
    to_write.append(str(len(ls)) + '\n')
    for x, y in ls:
        to_write.append(str(x) + ' ' + str(y) + '\n')

    with open(f'Countries/{country_name}.txt', 'w') as f:
        f.writelines(to_write)