Import("env", "projenv")

# access to global build environment
print(env)

# access to project build environment (is used source files in "src" folder)
print(projenv)
#
# Upload actions
#

def before_upload(source, target, env):
    if source[0].get_size() > 490000:
        raise Exception(f"File size ({source[0].get_size()}) excceds maximum limit to allow OTA") 

#print("Current build targets", map(str, BUILD_TARGETS))

env.AddPreAction("upload", before_upload)
