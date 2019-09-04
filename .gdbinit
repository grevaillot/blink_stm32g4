python
import time

def pwr(arg):
  try:
    ret = gdb.execute("monitor tpwr", to_string=True)
    if (arg == 0):
      print ("disable pwr")
      gdb.execute("mon tpwr disable")
    else:
      if "disabled" in ret:
       gdb.execute("mon tpwr enable")
       time.sleep(1)
  except:
    pass

def do_try(arg):
  try:
  	gdb.execute(arg)
  except:
  	print("could not execute " + arg)

def scan_and_attach():
	gdb.execute("monitor swdp_scan")
	do_try("attach 1")
	do_try("monitor vector_catch enable int bus stat chk mm reset hard")

end

define scana
	python scan_and_attach()
end

#!reset
set history filename .gdb_history
set history save

set target-async on
target extended-remote 127.0.0.1:3333

set mem inaccessible-by-default off
set print pretty

b cm3_assert_failed

