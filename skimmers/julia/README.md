# Setup
Once you have Julia installed, `cd` to this directory, and run
```
$> julia --project=.
# press `]` to go into Pkg> mode
pkg> instantiate
```

Later, launch julia with `--project=.` will give you access to all the neede packages.

# How to convert nanoAOD to uncompressed Arrow
Start with a nanoAOD, you need to make an apache arrow file **without** compression.

## Option 1: Use Julia
Use Julia Arrow to .root -> uncompressed arrow:
```julia
using UnROOT, DataFrames, Arrow
const lt = LazyTree("/tmp/0A0C246F-D01B-6F4D-85E6-3A75C27C5197.root", "Events");
df = DataFrame(lt; copycols=false)
Arrow.write("./nanoAOD_nocomp.feather", @view df[1:5*10^5, :])
```
## Option 2: Use Python
```python
In [2]: import uproot as up, awkward as ak, pyarrow.feather

In [3]: e = up.open("/tmp/0A0C246F-D01B-6F4D-85E6-3A75C27C5197.root")["Events"]

In [5]: df = e.arrays(entry_stop=5*10**3)

In [6]: arrow_table = ak.to_arrow_table(df)

In [7]: pyarrow.feather.write_feather(arrow_table, "./nanoAOD_nocomp.feather", compression="uncompressed"))
```

# How to benchmark and developer this package:
```julia
$> julia --project=. # inside this folder

julia> using Revise

julia> includet("./main.jl")

julia> main(dfa); # once to compile everything

julia> @time main(dfa);
  0.625138 seconds (5.92 M allocations: 361.897 MiB, 10.00% gc time)
```

# Size comparison
```bash
> ll "/home/akako/Downloads/nanoAOD_nocomp.feather"
4.0G Apr  1 10:51 /home/akako/Downloads/nanoAOD_nocomp.feather

> ll output.feather
1.2M Apr  1 19:51 output.feather


```
