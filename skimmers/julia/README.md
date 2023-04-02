# How to prepare input file
Start with a nanoAOD, you need to make an apache arrow file **without** compression.

I used a convoluted two-step process because some missing capability of UnROOT, and my inability to
use `pyarrow` to write uncompressed file...

## Step-1
use `uproot`, `awkward`, and `pyarrow`:
```python
import uproot as up
import awkward as ak
import pyarrow.feather

array = up.open("./0A0C246F-D01B-6F4D-85E6-3A75C27C5197.root")["Events"].arrays(entry_stop=500000);

df = ak.to_arrow_table(array);

pyarrow.feather.write_feather(df, "./nanoAOD.feather")
```

## Step-2
Use Julia Arrow to re-write again, removes compression:
```julia
using Arrow
dfa = Arrow.Table("./nanoAOD.feather");
Arrow.write("./nanoAOD_nocomp.feather", dfa)
```


# How to benchmark and developer this package:
```julia
$> julia --project=. # inside this folder

# pree `]` to get into Pkg mode, and `backspace` to back

pkg> instantiate

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
