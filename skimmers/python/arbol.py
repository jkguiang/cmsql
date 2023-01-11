import fnmatch
import uproot
import awkward as ak

class Arbol:
    def __init__(self, output_file):
        self.output_file = output_file

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        # make sure the dbconnection gets closed
        self.output_file.close()

    @staticmethod
    def __is_root_compatible(array):
        """
        Returns if array is a flat or 1-d jagged array

        Stolen from: 
        https://gitlab.cern.ch/gagarwal/ttbardileptonic/-/blob/master/TTbarDileptonSkim.py#L41-49
        """
        t = ak.type(array)
        if isinstance(t, ak._ext.ArrayType):
            if isinstance(t.type, ak._ext.PrimitiveType):
                return True
            if isinstance(t.type, ak._ext.ListType) and isinstance(t.type.type, ak._ext.PrimitiveType):
                return True
        return False

    @staticmethod
    def __ak_packed(data):
        return ak.packed(ak.without_parameters(data))

    @classmethod
    def create(cls, output_file_name):
        return cls(uproot.create(output_file_name))

    @classmethod
    def recreate(cls, output_file_name):
        return cls(uproot.recreate(output_file_name))

    def write(self, events, branch_names=None, ttree_name="Events"):
        ttree = {}
        if branch_names:
            all_branch_names = dir(events)
            expanded_wildcards = {}
            for branch_name in branch_names:
                if "*" in branch_name:
                    expanded_wildcards[branch_name] = fnmatch.filter(all_branch_names, branch_name)

            for wildcard_branch, found_branches in expanded_wildcards.items():
                wildcard_i = branch_names.index(wildcard_branch)
                branch_names = branch_names[:wildcard_i] + found_branches + branch_names[wildcard_i+1:]
        else:
            branch_names = events.fields

        for branch_name in branch_names:
            if hasattr(events, branch_name):
                branch = events[branch_name]
                if not branch.fields:
                    ttree[branch_name] = self.__ak_packed(branch)
                else:
                    packed_branch = {}
                    for field in branch.fields:
                        if self.__is_root_compatible(branch[field]):
                            packed_branch[field] = self.__ak_packed(branch[field])
                    ttree[branch_name] = ak.zip(packed_branch)
            else: 
                raise Exception(f"'{branch}' is not avaiable in the field names.")

        self.output_file[ttree_name] = ttree
