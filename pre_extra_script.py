Import("env")

# access to global build environment
print(env)

# Build actions

print("BEFORE BUILD----------------------")

print("Running gulp")
env.Execute("cd web && gulp")

#env.Command('bar.out', 'bar.in',
#            ["rm -f $TARGET",
#             "$BAR_BUILD < $SOURCES > $TARGET"],
#            ENV = {'PATH' : '/usr/local/bin/'})
