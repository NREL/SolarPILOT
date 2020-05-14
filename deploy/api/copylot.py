import string, sys, os
from ctypes import *

c_number = c_double   #must be either c_double or c_float depending on copilot.h definition

class CoPylot:
    def __init__(self):
        if sys.platform == 'win32' or sys.platform == 'cygwin':
            self.pdll = CDLL("./solarpilot.dll")
        elif sys.platform == 'darwin':
            self.pdll = CDLL("./solarpilot.dylib")
        elif sys.platform == 'linux2':
            self.pdll = CDLL("./solarpilot.so")
        else:
            print( 'Platform not supported ', sys.platform)

    def data_create(self):
        self.pdll.sp_data_create.restype = c_void_p
        return self.pdll.sp_data_create()

        #sp_set_value(sp_data_t p_data, const char* name, sp_number_t v);
    def data_set_number(self, p_data, name, value):
        self.pdll.sp_set_value(c_void_p(p_data), c_char_p(name), c_number(value))

    def data_get_number(self, p_data, name):
        self.pdll.data_get_number.restype = c_number
        return self.pdll.sp_get_value.sp_get_number(c_void_p(p_data), c_char_p(name))

#-----------------------------------------------------------------
if __name__ == "__main__":

    cp = CoPylot()

    r = cp.data_create()

    cp.data_set_number(r, "solarfield.0.dni_des", 1111.)

    print ( cp.data_get_number(r, "solarfield.0.dni_des") )