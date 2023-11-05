# Set up
```bash
>pwd
./cmsql_main/skimmers/julia
```
```julia
> julia --project=.
# press ] to enter Pkg mode
# only need instantiate first time
(CMSql) pkg> instantiate
Precompiling project...
  1 dependency successfully precompiled in 12 seconds. 60 already precompiled.
  1 dependency had warnings during precompilation:
┌ CMSql [eb8f4926-4e9b-498b-91c8-c899330fbaad]
│      CondaPkg Found dependencies: /home/akako/Documents/github/cmsql_main/skimmers/julia/CondaPkg.toml
│      CondaPkg Found dependencies: /home/akako/.julia/packages/PythonCall/1f5yE/CondaPkg.toml
│      CondaPkg Resolving changes
└
```

# How to prepare input file
Start with a nanoAOD, you need to make an apache arrow file **without** compression.

```julia
using UnROOT, DataFrames, Arrow
const lt = LazyTree("/tmp/0A0C246F-D01B-6F4D-85E6-3A75C27C5197.root", "Events");
df = DataFrame(lt; copycols=false)
Arrow.write("./nanoAOD_nocomp.feather", df)
```

# Skim from feather and back to `.root`

```julia
julia> using CMSql

julia> skim_feather("./nanoAOD_nocomp.feather"; output="./output.feather")
"./output.feather"

julia> @time skim_feather("./nanoAOD_nocomp.feather"; output="./output.feather")
  1.146882 seconds (7.18 M allocations: 422.560 MiB, 7.67% gc time)
"./output.feather"

julia> @time feather_to_root("./output.feather"; output="./output.root")
 10.776623 seconds (3.67 k allocations: 70.930 KiB)
"./output.root"
```

# Size comparison

```bash
-rw-r--r-- 1 akako akako 4.9G Apr  3 00:03 nanoAOD_nocomp.feather
-rw-r--r-- 1 akako akako 1.2M May 19 11:29 output.feather
-rw-r--r-- 1 akako akako 1.8M May 19 11:29 output.root
```
