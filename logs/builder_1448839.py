# 2026-09-27T06:59:19.549579
import vitis

client = vitis.create_client()
client.set_workspace(path="/nfs_scratch/dasu/2026-08/geawis")

vitis.dispose()

