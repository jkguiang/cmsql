# Setup
Once you have Julia installed, `cd` to this directory, and run
```
$> julia --project=.
# press `]` to go into Pkg> mode
pkg> instantiate
```

Later, launch julia with `--project=.` will give you access to all the neede packages.

# How to prepare input file
Start with a nanoAOD, you need to make an apache arrow file **without** compression.

## Converting nanoAOD to uncompressed Arrow
Use Julia Arrow to .root -> uncompressed arrow:
```julia
using UnROOT, DataFrames, Arrow
const lt = LazyTree("/tmp/0A0C246F-D01B-6F4D-85E6-3A75C27C5197.root", "Events");
df = DataFrame(lt; copycols=false)
Arrow.write("./nanoAOD_nocomp.feather", @view df[1:5*10^5, :])
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
