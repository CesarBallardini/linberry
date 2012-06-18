import bb_usb
import bb_data
import bb_messenging

bb_messenging.verbose=True
device = bb_usb.find_berry()
device.open_handle()
device.read_endpoints(0)
device.claim_interface()
data=[0x48,0x65,0x6c,0x6c,0x6f,0x20,0x77,0x6f,0x72,0x6c,0x64,0x21,0xa]
bb_usb.usb_write(device, device.writept, data, 60, "Data -> ")
device.release_interface()
