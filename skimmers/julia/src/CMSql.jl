module CMSql

using Arrow, DataFrames, LorentzVectorHEP

export skim_feather, feather_to_root

include("./main.jl")

using PythonCall

function feather_to_root(path; output)
    up = @pyconst(pyimport("uproot"))
    feather = @pyconst(pyimport("pyarrow.feather"))
    ak = @pyconst(pyimport("awkward"))

    a = ak.from_arrow(feather.read_table(path))
    pywith(up.recreate(output)) do f_out
        f_out["Events"] = PyDict(k => a[k] for k in a.fields)
    end
    output
end


### for precompile
# let 
#      tmp = tempname()
#      skim_feather("$(@__DIR__)/../nanoAOD_nocomp.feather"; output=tmp)
#      rm(tmp)
# end


end # module CMSql
