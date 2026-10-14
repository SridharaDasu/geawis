# 2026-09-27T07:07:15.219987
import vitis

client = vitis.create_client()
client.set_workspace(path="/nfs_scratch/dasu/2026-08/geawis")

comp = client.create_hls_component(name = "geawis_algo_top",cfg_file = ["hls_config.cfg"],template = "empty_hls_component")

comp = client.get_component(name="geawis_algo_top")
comp.run(operation="C_SIMULATION")

comp.run(operation="SYNTHESIS")

comp = client.get_component(name="algo_top")
comp.run(operation="SYNTHESIS")

comp.run(operation="C_SIMULATION")

comp.run(operation="SYNTHESIS")

vitis.dispose()

