import string, sys, os
from ctypes import *
c_number = c_double   #must be either c_double or c_float depending on copilot.h definition


@CFUNCTYPE(c_int, c_number, c_char_p)
def api_callback(fprogress, msg):
    if fprogress == 0:
        print("C++ API message -> {:s}".format(msg.decode()))
    else:
        print("Progress is {:f}\nC++ API message -> {:s}".format(fprogress,msg.decode()))
    return 1

class CoPylot:
    def __init__(self):
        if sys.platform == 'win32' or sys.platform == 'cygwin':
            self.pdll = CDLL("C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/build_vs2017/build/Debug/x64/solarpilot.dll")
            #self.pdll = CDLL("./solarpilot.dll")
        elif sys.platform == 'darwin':
            self.pdll = CDLL("./solarpilot.dylib")
        elif sys.platform == 'linux2':
            self.pdll = CDLL("./solarpilot.so")
        else:
            print( 'Platform not supported ', sys.platform)

    def api_callback_create(self,p_data):
        self.pdll.sp_set_callback(c_void_p(p_data), api_callback)

        # TODO: Add this function into C++ side of API
    def version(self):
        self.pdll.sp_version.restype = c_int
        return self.pdll.sp_version()

    def data_create(self):
        self.pdll.sp_data_create.restype = c_void_p
        return self.pdll.sp_data_create()

    #SPEXPORT void sp_set_number(sp_data_t p_data, const char* name, sp_number_t v);
    def data_set_number(self, p_data, name, value):
        self.pdll.sp_set_number(c_void_p(p_data), c_char_p(name), c_number(value)) 

    #SPEXPORT void sp_set_string(sp_data_t p_data, const char *name, const char *value)
    def data_set_string(self, p_data, name, svalue):
        self.pdll.sp_set_string(c_void_p(p_data), c_char_p(name), c_char_p(svalue))

    #SPEXPORT void sp_set_array(sp_data_t p_data, const char *name, sp_number_t *pvalues, int length)
    def data_set_array(self, p_data, name, parr):
        count = len(parr)
        arr = (c_number*count)()
        arr[:] = parr # set all at once
        return self.pdll.sp_set_array(c_void_p(p_data), c_char_p(name), pointer(arr), c_int(count))

    #SPEXPORT void sp_set_matrix(sp_data_t p_data, const char *name, sp_number_t *pvalues, int nrows, int ncols)
    def data_set_matrix(self, p_data, name, mat):
        nrows = len(mat)
        ncols = len(mat[0])
        size = nrows*ncols
        arr = (c_number*size)()
        idx = 0
        for r in range(nrows):
            for c in range(ncols):
                arr[idx] = c_number(mat[r][c])
                idx += 1
        return self.pdll.sp_set_matrix( c_void_p(p_data), c_char_p(name), pointer(arr), c_int(nrows), c_int(ncols))

    # Set matrix variable values through a csv file
    def data_set_matrix_from_csv(self, p_data, name, fn):
        f = open(fn, 'r', encoding="utf-8-sig")
        data = [] 
        for line in f : 
            lst = ([n for n in map(float, line.split(','))])
            data.append(lst)
        f.close()
        return self.data_set_matrix(p_data, name, data) 

    #SPEXPORT sp_number_t sp_get_number(sp_data_t p_data, const char* name)
    def data_get_number(self, p_data, name):
        self.pdll.sp_get_number.restype = c_number
        return self.pdll.sp_get_number(c_void_p(p_data), c_char_p(name))

    #SPEXPORT const char *sp_get_string(sp_data_t p_data, const char *name)
    def data_get_string(self, p_data, name):
        self.pdll.sp_get_string.restype = c_char_p
        return self.pdll.sp_get_string(c_void_p(p_data), c_char_p(name))
        # TODO: returning '' I am using the same format as SCC.  Have Mike check to see if the scc function works.

    #SPEXPORT sp_number_t *sp_get_array(sp_data_t p_data, const char *name, int *length)
    def data_get_array(self, p_data, name):
        count = c_int()
        self.pdll.sp_get_array.restype = POINTER(c_number)
        parr = self.pdll.sp_get_array(c_void_p(p_data), c_char_p(name), byref(count))
        arr = parr[0:count.value]
        return arr

    #Sets array using csv - TODO: test to see if it works
    def data_set_array_from_csv(self, p_data, name, fn):
        f = open(fn, 'r')
        data = []
        for line in f:
            data.extend([n for n in map(float, line.split(','))])
        f.close()
        return self.data_set_array(p_data, name, data)

    #SPEXPORT sp_number_t *sp_get_matrix(sp_data_t p_data, const char *name, int *nrows, int *ncols)
    def data_get_matrix(self,p_data,name):
        nrows = c_int()
        ncols = c_int()
        self.pdll.sp_get_matrix.restype = POINTER(c_number)
        parr = self.pdll.sp_get_matrix( c_void_p(p_data), c_char_p(name), byref(nrows), byref(ncols) )
        mat = []
        for r in range(nrows.value):
            row = []
            for c in range(ncols.value):
                row.append( float(parr[ncols.value * r + c]))
            mat.append(row)
        return mat

    #SPEXPORT void sp_reset_geometry(sp_data_t p_data)
    def reset_vars(self, p_data):
        return self.pdll.sp_reset_geometry( c_void_p(p_data))

    #SPEXPORT int sp_add_receiver(sp_data_t p_data, const char* receiver_name)
    def add_receiver(self, p_data, rec_name):
        self.pdll.sp_add_receiver.restype = c_int
        return self.pdll.sp_add_receiver( c_void_p(p_data), c_char_p(rec_name))

    #SPEXPORT int sp_drop_receiver(sp_data_t p_data, const char* receiver_name)
    def drop_receiver(self, p_data, rec_name):
        self.pdll.sp_drop_receiver.restype = c_int
        return self.pdll.sp_drop_receiver( c_void_p(p_data), c_char_p(rec_name))

    #SPEXPORT int sp_add_heliostat_template(sp_data_t p_data, const char* heliostat_name)
    def add_heliostat_template(self, p_data, helio_name):
        self.pdll.sp_add_heliostat_template.restype = c_int
        return self.pdll.sp_add_heliostat_template( c_void_p(p_data), c_char_p(helio_name))

    #SPEXPORT int sp_drop_heliostat_template(sp_data_t p_data, const char* heliostat_name)
    def drop_heliostat_template(self, p_data, helio_name):
        self.pdll.sp_drop_heliostat_template.restype = c_int
        return self.pdll.sp_drop_heliostat_template( c_void_p(p_data), c_char_p(helio_name))

        

#-----------------------------------------------------------------
if __name__ == "__main__":

    cp = CoPylot()

    print('Process ID: ', os.getpid())

    r = cp.data_create()
    cp.api_callback_create(r)

    ## Testing set and get number - DONE
    if False:
        cp.data_set_number(r, b"solarfield.0.dni_des", 1111.)
        print ( cp.data_get_number(r, b"solarfield.0.dni_des"))

    ## Testing set and get string - TODO
    if False:
        cp.data_set_string(r, b"ambient.0.loc_state", b"Mind")  #- Working
        print ( cp.data_get_string(r, b"ambient.0.loc_state"))

    ## Testing set and get array - DONE
    if False:
        cp.data_set_array(r, b"financial.0.pmt_factors", [1.,2.,3.,4.,5.,6.,7.,8.,9.])
        print( cp.data_get_array(r, b"financial.0.pmt_factors"))

    ## Testing set and get matrix - DONE
    if False:
        cp.data_set_matrix(r, b"ambient.0.atm_coefs", [[1,1,1,1],[2,2,2,2],[3,3,3,3]])
        #cp.data_set_matrix_from_csv(r, b"ambient.0.atm_coefs", "test.csv")
        print( cp.data_get_matrix(r, b"ambient.0.atm_coefs"))

    ## Testing variable reset - DONE
    if False:
        cp.reset_vars(r)
        print ( cp.data_get_number(r, b"solarfield.0.dni_des"))
        print( cp.data_get_array(r, b"financial.0.pmt_factors"))
        print( cp.data_get_matrix(r, b"ambient.0.atm_coefs"))

    ## Testing add and drop receiver
    if False:
        print( cp.add_receiver(r, b"test_rec"))
        print( cp.add_receiver(r, b"test_rec"))
        print( cp.drop_receiver(r, b"test_rec"))
        print( cp.drop_receiver(r, b"test_re"))

    ## Testing add and drop heliostat template
    if True:
        print( cp.add_heliostat_template(r, b"test_helio"))
        print( cp.add_heliostat_template(r, b"test_helio"))
        print( cp.drop_heliostat_template(r, b"test_helio"))
        print( cp.drop_heliostat_template(r, b"test_heli"))







