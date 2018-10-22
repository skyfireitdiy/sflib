import os

version = input("input version:")
version_info = []
print("version info:")
while True:
    tmp_info = input()
    if tmp_info == "exit":
        break
    version_info.append(tmp_info)

add_info = '''
/**
* @version ''' + version + '''
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
'''

last_msg = '\n* ' + '\n* '.join(version_info) + '\n*/\n\n'

comfirm = input("input yes to continue:")
if comfirm != "yes":
    exit(0)

g = os.walk(".")  

for path,dir_list,file_list in g:  
    for file_name in file_list:  
        name = os.path.join(path, file_name)
        if name.endswith(".h") or name.endswith(".hpp") or name.endswith(".cpp"):
            new_content = ""
            with open(name, 'r') as fp:
                content = fp.read() 
                new_info = add_info + '''* @file '''+ file_name + '\n'
                new_content = new_info + last_msg + content
            if len(new_content) != 0:
                with open(name, 'w') as fp:
                    fp.write(new_content)
                print(name + " resolved")
