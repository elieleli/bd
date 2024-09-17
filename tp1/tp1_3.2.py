import sys
import re

def process_line(line):
    parts = line.replace(':','').split()
    return parts

def get_value(word_value):
    value = word_value[1]
    return value

def get_title(word_title):
    title = ' '.join(word_title[1:])
    return title

def get_similar(word_similar):
    similar = word_similar[2:]
    return similar

#OBS : mesma categoria com dois ids, precisamos tratar
#OBS2: ver oq fica melhor como chave
def process_categories(unpro_categories):
    categories_list = []
    seen_id = []
    #the paramater is a list with all the lines of categories
    for line in unpro_categories:
        #take one line at time and divide the categories
        categories = line.replace("\n", "").split("|")

        for category  in categories[1:]:
            #check if is not empthy
            parts = category.split('[')

            # Check if the string has multiple brackets (special case)
            if len(parts) == 3:
                # Handle special case where you have two brackets
                name = parts[0].strip() + ', ' + parts[1].replace(']', '').strip()  # "Williams, John, guitar"
                id = parts[2].replace(']', '').strip()  # "63054"
            elif len(parts) == 2:
                # Handle regular case where only one bracket exists
                name = parts[0].strip()
                id = parts[1].replace(']', '').strip()
            else:
                # Handle cases where the format doesn't match expected patterns
                name = category
                id = None
                
            if (id not in seen_id) and name:
                categories_list.append({"id": to_int(id), "name": name})
                seen_id.append(id)
                
            
    return categories_list

def is_date(string_date): 
    if re.match(r'^\d{4}-\d{1,2}-\d{1,2}$', string_date): # 'YYYY-M-D' date format, with 4 dig. year, 1-2 dig. month, and 1-2 digs. day.
        year, month, day = map(int, string_date.split('-'))
        return year, month, day
    else:
        return None

def to_int(value):
    #returns a list of the match
    number_str = re.findall(r'\d+', value)
    #check if is has somenthing
    if number_str:  
        #transform to int
        return int(number_str[0])  
    return None  


def describe_product(lineF, index_line): 

    #Mapping keys to processing functions
    processing_map = { 
        "ASIN": get_value,
        "title": get_title,
        "group": get_value,
        "salesrank": get_value,
    }

    product_info = {}
    similar_asin = []
    reviews = []
    category_list = []
    prod_subcategory =[]

    while index_line < len(lineF):
        #list of strings from one line
        line_processed = process_line(lineF[index_line]) 
        if line_processed:
            key = line_processed[0]
            
            if key in processing_map:
                #function from the map processing
                process_function = processing_map[key] 
                product_info[key.lower()] = process_function(line_processed)
                index_line += 1

            #case for similar: the function returns two values: one for the product info (total) and one for the similar asin dic (a list with the "number" of the asin) 
            elif key == "similar":
                #if the categories is 0, the rest os list is empthy
                if len(line_processed[2:])>0:
                    for similar in get_similar(line_processed):
                        #key is the product and the value is the similar product
                        similar_asin.append({"asin":to_int(product_info["asin"]), "asin_similar":to_int(similar) })
                index_line+=1

            #special case: save the categories/sub categories for the prodduct, the key is (id, asin)
            elif key == "categories":
                #list to save each line of categorie
                categories_list = []
                while True:
                    index_line += 1
                    line_categorie = lineF[index_line]

                    #if is not empthy or in review section stops
                    if not line_categorie or ("reviews" in line_categorie):
                        index_line -= 1
                        break
                    
                    #just get each line of categorie
                    categories_list.append(line_categorie)

                if categories_list:
                    #for each line of categorie, this function get the id and name of the categories and return a dic
                    category_list = process_categories(categories_list)
                    
                    #get the first category 
                    main_category = category_list[0]
                    prod_category={"asin": to_int(product_info['asin']), "id_category":main_category["id"]}
                    
                    #get the subcategories
                    for category in category_list[1:]:
                        prod_subcategory.append({"asin": to_int(product_info['asin']), 
                                                 "id_category":main_category["id"], 
                                                 "id_subcategory":category["id"] })
                    

                index_line += 1

            elif key == "reviews":
                index_line += 1
            
            #make the verification and already get the values
            elif date := is_date(key):
                    year, month, day = date
                    comment = {
                        "asin": to_int(product_info['asin']),
                        "year": year,
                        "month": month,
                        "day": day,
                        line_processed[1]: line_processed[2],
                        line_processed[3]: to_int(line_processed[4]),
                        line_processed[5]: to_int(line_processed[6]),
                        line_processed[7]: to_int(line_processed[8])
                    }
                    index_line+=1
                    reviews.append(comment)
            
            else:
                break
        else:
            break
    
     
    if category_list:
        print("--------Categories:--------")
        for i in category_list:
            print(i)
    
    print("\n")
    # Informações adicionais
    """ print(f"inputfile pos {index_line}: {lineF[index_line-1]} and {process_line(lineF[index_line-1])}")
    if index_line < len(lineF):
        print(f"prox line: {lineF[index_line]}") """
    
    print('PRODUCT INFO')
    print(product_info)
    print("MAIN CATEGORY")
    print(prod_category)
    print("SUBCATEGORY")
    for i in prod_subcategory:
        print(i)
    print('PRODUCT SIMILAR')
    for i in similar_asin:
        print(i)
    print('REVIEWS')
    for i in reviews:
        print(i)

    return index_line, product_info

def process_file(input_file):
    with open(input_file, "r") as inputF:
        linesF = inputF.readlines()
        index_line = 0

        while index_line < len(linesF):
            line_processed = process_line(linesF[index_line])
            if line_processed:
                if "ASIN" in line_processed[0]:
                    describe_product(linesF, index_line)
            index_line +=1  

input_file = sys.argv[1]
process_file(input_file)