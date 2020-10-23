import sys, os
import pandas as pd
from ctypes import *
c_number = c_double   #must be either c_double or c_float depending on copilot.h definition

@CFUNCTYPE(c_int, c_number, c_char_p)
def api_callback(fprogress, msg):
    newline = False
    if fprogress != 0:
        print("Progress is {:.2f} %".format(fprogress*100))
        newline = True
    if msg.decode() != '':
        if newline:
            print("\n")
        print("C++ API message -> {:s}".format(msg.decode()))
    return 1

# TODO: Can build both dll and lib file using a post processing step
# https://stackoverflow.com/questions/376296/building-both-dll-and-static-libs-from-the-same-project#:~:text=There%20is%20an%20easy%20way,lib%20versions%20in%20one%20project.&text=Then%2C%20in%20your%20project%2C%20add,the%20command%20just%20runs%20the%20.&text=Then%2C%20you%20will%20always%20get,dll%20and%20a%20static%20lib.

class CoPylot:
    def __init__(self):
        cwd = os.getcwd()
        if sys.platform == 'win32' or sys.platform == 'cygwin':
            #self.pdll = CDLL("C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/build_vs2017/build/Debug/x64/solarpilot.dll") # for debugging
            self.pdll = CDLL(cwd + "/solarpilot.dll")
        elif sys.platform == 'darwin':
            self.pdll = CDLL(cwd + "/solarpilot.dylib")
        elif sys.platform == 'linux2':
            self.pdll = CDLL(cwd +"/solarpilot.so")
        else:
            print( 'Platform not supported ', sys.platform)

    def api_callback_create(self,p_data):
        self.pdll.sp_set_callback(c_void_p(p_data), api_callback)

    def api_disable_callback(self,p_data):
        self.pdll.sp_disable_callback(c_void_p(p_data))

    def version(self, p_data):
        self.pdll.sp_version.restype = c_char_p
        return self.pdll.sp_version(c_void_p(p_data) ).decode()

    def data_create(self):
        self.pdll.sp_data_create.restype = c_void_p
        return self.pdll.sp_data_create()

    def data_free(self, p_data):
        self.pdll.sp_data_free(c_void_p(p_data))

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

    # Set array variable through a csv file
    def data_set_array_from_csv(self, p_data, name, fn):
        f = open(fn, 'r', encoding="utf-8-sig")
        data = []
        for line in f:
            data.extend([n for n in map(float, line.split(','))])
        f.close()
        return self.data_set_array(p_data, name, data)

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

    #SPEXPORT sp_number_t *sp_get_array(sp_data_t p_data, const char *name, int *length)
    def data_get_array(self, p_data, name):
        count = c_int()
        self.pdll.sp_get_array.restype = POINTER(c_number)
        parr = self.pdll.sp_get_array(c_void_p(p_data), c_char_p(name), byref(count))
        arr = parr[0:count.value]
        return arr

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

    #SPEXPORT int sp_update_geometry(sp_data_t p_data)
    def update_geometry(self, p_data):
        self.pdll.sp_update_geometry.restype = c_int
        return self.pdll.sp_update_geometry( c_void_p(p_data))

    #SPEXPORT bool sp_generate_layout(sp_data_t p_data, int nthreads = 0)
    def generate_layout(self, p_data, nthreads = 0):
        self.pdll.sp_generate_layout.restype = c_bool
        return self.pdll.sp_generate_layout( c_void_p(p_data), c_int(nthreads))

    #SPEXPORT bool sp_assign_layout(sp_data_t p_data, sp_number_t* pvalues, int nrows, int ncols, int nthreads = 0) //, bool save_detail = true)
    def assign_layout(self, p_data, helio_data, nthreads = 0):
        nrows = len(helio_data)
        ncols = len(helio_data[0])
        size = nrows*ncols
        arr = (c_number*size)()
        idx = 0
        for r in range(nrows):
            for c in range(ncols):
                arr[idx] = c_number(helio_data[r][c])
                idx += 1
        self.pdll.sp_assign_layout.restype = c_bool
        return self.pdll.sp_assign_layout( c_void_p(p_data), pointer(arr), c_int(nrows), c_int(ncols), c_int(nthreads))

    #SPEXPORT sp_number_t* sp_get_layout_info(sp_data_t p_data, int* nhelio, int* ncol, bool get_corners = false)
    def get_layout_info(self, p_data, get_corners = False, restype = "dataframe"):
        nrows = c_int()
        ncols = c_int()
        # Get data
        self.pdll.sp_get_layout_info.restype = POINTER(c_number)
        parr = self.pdll.sp_get_layout_info( c_void_p(p_data), byref(nrows), byref(ncols),  c_bool(get_corners))
        # Get header
        self.pdll.sp_get_layout_header.restype = c_char_p
        header = self.pdll.sp_get_layout_header( c_void_p(p_data), c_bool(get_corners)).decode()
        header = header.split(',')
        if restype.lower().startswith("mat"):
            # output matrix
            mat = []
            for r in range(nrows.value):
                row = []
                for c in range(ncols.value):
                    row.append( float(parr[ncols.value * r + c]))
                mat.append(row)
            return mat, header
        elif restype.lower().startswith("dic") or restype.lower().startswith("dat"):
            # output dictionary
            ret = {}
            for c,key in enumerate(header):
                ret[key] = []
                for r in range(nrows.value):
                    ret[key].append( float(parr[ncols.value * r + c]))
            if restype.lower().startswith("dic"):
                return ret
            else:
                df = pd.DataFrame(ret)
                df.set_index(header[0])
                return df

    #SPEXPORT bool sp_simulate(sp_data_t p_data, int nthreads = 1, bool update_aimpoints = true)
    def simulate(self, p_data, nthreads = 1, update_aimpoints = True):
        self.pdll.sp_simulate.restype = c_bool
        return self.pdll.sp_simulate( c_void_p(p_data), c_int(nthreads), c_bool(update_aimpoints))

    #SPEXPORT const char *sp_summary_results(sp_data_t p_data)
    def summary_results(self, p_data, save_dict = False):
        self.pdll.sp_summary_results.restype = c_char_p
        ret = self.pdll.sp_summary_results( c_void_p(p_data)).decode()
        # save result table to dictionary
        if save_dict:
            items = ret.split('\n')
            res_dict = {}
            for i in items:
                key_val = i.split(',')
                try:
                    res_dict[key_val[0]] = float(key_val[1])
                except:
                    res_dict[key_val[0]] = key_val[1]
            return res_dict
        else:    # print results table
            return print(ret)

    #SPEXPORT sp_number_t* sp_detail_results(sp_data_t p_data, int* nrows, int* ncols, sp_number_t* selhel = NULL, int nselhel = 0)
    def detail_results(self, p_data, selhel = None, restype = "dataframe", get_corners = False):
        # handling selected heliostats
        if selhel == None:
            nselhel = 0
            arr = c_number(0)
        else:
            nselhel = len(selhel)
            arr = (c_number*nselhel)()
            arr[:] = selhel # set all at once

        nrows = c_int()
        ncols = c_int()
        # Get data
        self.pdll.sp_detail_results.restype = POINTER(c_number)
        res_arr = self.pdll.sp_detail_results( c_void_p(p_data), byref(nrows), byref(ncols), pointer(arr), c_int(nselhel), c_bool(get_corners))
        try:
            # Get header
            self.pdll.sp_detail_results_header.restype = c_char_p
            header = self.pdll.sp_detail_results_header( c_void_p(p_data), c_bool(get_corners)).decode()
            header = header.split(',')
            if restype.lower().startswith("mat"):
                # output matrix
                mat = []
                for r in range(nrows.value):
                    row = []
                    for c in range(ncols.value):
                        row.append( float(res_arr[ncols.value * r + c]))
                    mat.append(row)
                return mat, header
            elif restype.lower().startswith("dic") or restype.lower().startswith("dat"):
                # output dictionary
                ret = {}
                for c,key in enumerate(header):
                    ret[key] = []
                    for r in range(nrows.value):
                        ret[key].append( float(res_arr[ncols.value * r + c]))
                if restype.lower().startswith("dic"):
                    return ret
                else:
                    df = pd.DataFrame(ret)
                    df.set_index(header[0])
                    return df
        except:
            print("Detailed results API called failed to return correct information.")

    #SPEXPORT sp_number_t* sp_get_fluxmap(sp_data_t p_data, int* nrows, int* ncols, int rec_id = 0)
    def get_fluxmap(self, p_data, rec_id = 0):
        nrows = c_int()
        ncols = c_int()
        self.pdll.sp_get_fluxmap.restype = POINTER(c_number)
        res = self.pdll.sp_get_fluxmap( c_void_p(p_data), byref(nrows), byref(ncols), c_int(rec_id))
        # output matrix
        mat = []
        for r in range(nrows.value):
            row = []
            for c in range(ncols.value):
                row.append( float(res[ncols.value * r + c]))
            mat.append(row)
        return mat
    
    #SPEXPORT void sp_clear_land(sp_data_t p_data, const char* type = NULL)
    def clear_land(self, p_data, clear_type = None):
        return self.pdll.sp_clear_land( c_void_p(p_data), c_char_p(clear_type))

    #SPEXPORT bool sp_add_land(sp_data_t p_data, const char* type, sp_number_t* polygon_points, int* npts , int* ndim, bool is_append = true)
    def add_land(self, p_data, add_type, poly_points, is_append = True):
        npts = len(poly_points)
        ndim = len(poly_points[0])
        size = npts*ndim
        PParr = (c_number*size)()
        idx = 0
        for r in range(npts):
            for c in range(ndim):
                PParr[idx] = c_number(poly_points[r][c])
                idx += 1
        
        self.pdll.sp_add_land.restype = c_bool
        return self.pdll.sp_add_land( c_void_p(p_data), c_char_p(add_type), pointer(PParr), byref(c_int(npts)), byref(c_int(ndim)), c_bool(is_append))

    #SPEXPORT sp_number_t* sp_heliostats_by_region(sp_data_t p_data, const char* coor_sys, int* lenret,
    #                                                sp_number_t* arguments = NULL, int* len_arg = NULL,
    #                                                const char* svgfname_data = NULL, sp_number_t* svg_opt_tab = NULL);
    def heliostats_by_region(self, p_data, coor_sys = b'all', **kwargs):
        """
        Returns heliostats that fall within a region. Options are:\n
            >> all (no additional arguments),\n
            >> cylindrical (provide [rmin, rmax, azmin radians, azmax radians]),\n
            >> cartesian (provide [xmin, xmax, ymin, ymax[, zmin, zmax]]),\n
            >> polygon (provide [[x1,y1],[x2,y2],...]),\n
            >> svg (provide string with 'scale-x scale-y;offset-x offset-y;<svg path 1>;<svg path 2>;...',\n
            >> svgfile (provide string filename, optional table (array) [scale-x, scale-y, offset-x, offset-y]).\n
            (string:system, variant:region info[, string:return info - id/location])\n
        Returns an array of included heliostat ID's and locations. : matrix, dictionary, dataframe
        """
        argsdict = {
            'arguments': [],
            'svgfname_data': b'', 
            'svg_opt_tab' : [],
            'restype': 'dataframe'
        }
        argsdict.update(kwargs)

        # flatten polygon points
        if coor_sys == b'polygon':
            temp = []
            for r in range(len(argsdict['arguments'])):
                for c in range(len(argsdict['arguments'][0])):
                    temp.append(argsdict['arguments'][r][c])
            argsdict['arguments'] = temp
        
        len_arg = len(argsdict['arguments'])
        arg_arr = (c_number*len_arg)()
        arg_arr[:] = argsdict['arguments']

        lenret = c_int()
        self.pdll.sp_heliostats_by_region.restype = POINTER(c_number)
        if coor_sys == b'all':
            res = self.pdll.sp_heliostats_by_region( c_void_p(p_data), c_char_p(coor_sys), byref(lenret) )
        elif coor_sys == b'cylindrical' or coor_sys == b'cartesian' or coor_sys == b'polygon':
            res = self.pdll.sp_heliostats_by_region( c_void_p(p_data), c_char_p(coor_sys), byref(lenret), pointer(arg_arr), byref(c_int(len_arg)))
        elif coor_sys == b'svg' or coor_sys == b'svgfile':
            if len(argsdict['svg_opt_tab'] == 0):
                res = self.pdll.sp_heliostats_by_region( c_void_p(p_data), c_char_p(coor_sys), byref(lenret), pointer(arg_arr), byref(c_int(len_arg)), c_char_p(argsdict['svgfname_data']) )
            elif len(argsdict['svg_opt_tab']) == 4:
                svg_tab = (c_number*4)()
                svg_tab[:] = argsdict['svg_opt_tab']
                res = self.pdll.sp_heliostats_by_region( c_void_p(p_data), c_char_p(coor_sys), byref(lenret), pointer(arg_arr), byref(c_int(len_arg)), c_char_p(argsdict['svgfname_data']), pointer(svg_tab) )
            else:
                print('svg_opt_tab must have the following form: [scale-x, scale-y, offset-x, offset-y]')
        else:
            print('Invalid region type specified. Expecting one of [cylindrical, cartesian, polygon, svg, svgfile]')

        # Unpacking results - id, location x, location y, location z
        header = ['id','location-x','location-y','location-z']
        if argsdict['restype'].lower().startswith("mat"):
            # output matrix
            mat = []
            for r in range(int(lenret.value/4)):
                row = []
                for c in range(4):
                    row.append( float(res[4 * r + c]))
                mat.append(row)
            return mat, header
        elif argsdict['restype'].lower().startswith("dic") or argsdict['restype'].lower().startswith("dat"):
            # output dictionary
            ret = {}
            for c,key in enumerate(header):
                ret[key] = []
                for r in range(int(lenret.value/4)):
                    ret[key].append( float(res[4 * r + c]))
            if argsdict['restype'].lower().startswith("dic"):
                return ret
            else:
                df = pd.DataFrame(ret)
                df.set_index(header[0])
                return df

    #SPEXPORT bool sp_modify_heliostats(sp_data_t p_data, sp_number_t* helio_data, int* nhel, int* ncols, const char* table_hdr)
    def modify_heliostats(self, p_data, helio_dict):
        ## helio_data is a dictionary using a subset of the following keys
        """
            'id',               - Required
            'location-x',
            'location-y',
            'location-z',
            'aimpoint-x',
            'aimpoint-y',
            'aimpoint-z',
            'soiling',
            'reflectivity',
            'enabled'
        """
        ncols = len(helio_dict.keys())
        nhel = len(helio_dict[next(iter(helio_dict))])
        table_hdr = ""
        for key in helio_dict.keys():
            table_hdr += key
            if (key != list(helio_dict)[-1]):
                table_hdr += ','
        table_hdr = table_hdr.encode()
        
        size = ncols*nhel
        helio_data = (c_number*size)()
        idx = 0
        for h in range(nhel):
            for key in helio_dict.keys():
                helio_data[idx] = c_number(helio_dict[key][h])
                idx += 1

        self.pdll.sp_modify_heliostats.restype = c_bool
        return self.pdll.sp_modify_heliostats( c_void_p(p_data), pointer(helio_data), byref(c_int(nhel)), byref(c_int(ncols)), c_char_p(table_hdr) )

    #SPEXPORT bool sp_save_from_script(sp_data_t p_data, const char* sp_fname)
    def save_from_script(self, p_data, sp_fname):
        self.pdll.sp_save_from_script.restype = c_bool
        return self.pdll.sp_save_from_script( c_void_p(p_data), c_char_p(sp_fname))

    #SPEXPORT bool sp_dump_varmap(sp_data_t p_data, const char* sp_fname)
    def dump_varmap_tofile(self, p_data, sp_fname):
        self.pdll.sp_dump_varmap.restype = c_bool
        return self.pdll.sp_dump_varmap( c_void_p(p_data), c_char_p(sp_fname))
    