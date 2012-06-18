from LinB_EscanearMod import BBGui
import bb_messenging
# Main
gui = BBGui(0)
# enable feddback from bbtether to gui
bb_messenging.gui = gui
gui.MainLoop()
