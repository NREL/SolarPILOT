import sys, os
import pandas as pd
from ctypes import *
c_number = c_double   #must be either c_double or c_float depending on copilot.h definition

@CFUNCTYPE(c_int, c_number, c_char_p)
def api_callback(fprogress, msg):
    """Callback function for API -> prints message from SolarPILOT DLL"""
    newline = False
    if fprogress != 0:
        print("\rProgress is {:.2f} %".format(fprogress*100), end = "\r")
        newline = True
    if msg.decode() != '':
        if newline:
            print("\n")
        print("C++ API message -> {:s}".format(msg.decode()))
    return 1

class CoPylot:
    """
    A class to access CoPylot (SolarPILOT's Python API)

    Attributes
    ----------
    pdll : class ctypes.CDLL
        loaded SolarPILOT library of exported functions

    Methods
    -------
    version(p_data: int) -> str
        Provides SolarPILOT version number
    data_create() -> int
        Creates an instance of SolarPILOT in memory
    data_free(p_data: int) -> bool
        Frees SolarPILOT instance from memory
    api_callback_create(p_data: int) -> None
        Creates a callback function for message transfer
    api_disable_callback(p_data: int) -> None
        Disables callback function
    data_set_number(p_data: int, name: str, value) -> bool
        Sets a SolarPILOT numerical variable, used for float, int, bool, and numerical combo options.
    data_set_string(p_data: int, name: str, svalue: str) -> bool
        Sets a SolarPILOT string variable, used for string and combos
    data_set_array(p_data: int, name: str, parr: list) -> bool
        Sets a SolarPILOT array variable, used for double and int vectors
    data_set_array_from_csv(p_data: int, name: str, fn: str) -> bool
        Sets a SolarPILOT vector variable from a csv, used for double and int vectors
    data_set_matrix(p_data: int, name: str, mat: list) -> bool
        Sets a SolarPILOT matrix variable, used for double and int matrix
    data_set_matrix_from_csv(p_data: int, name: str, fn: str) -> bool
        Sets a SolarPILOT matrix variable from a csv, used for double and int matrix
    data_get_number(p_data: int, name: str) -> float
        Gets a SolarPILOT numerical variable value
    data_get_string(p_data: int, name: str) -> str
        Gets a SolarPILOT string variable value
    data_get_array(p_data: int, name: str) -> list
        Gets a SolarPILOT array (vector) variable value
    data_get_matrix(p_data: int,name: str) -> list
        Gets a SolarPILOT matrix variable value
    reset_vars(p_data: int) -> bool
        Resets SolarPILOT variable values to defaults
    add_receiver(p_data: int, rec_name: str) -> int
        Creates a receiver object
    drop_receiver(p_data: int, rec_name: str) -> bool
        Deletes a receiver object
    add_heliostat_template(p_data: int, helio_name: str) -> int
        Creates a heliostat template object
    drop_heliostat_template(p_data: int, helio_name: str) -> bool
        Deletes heliostat template object
    update_geometry(p_data: int) -> bool
        Refresh the solar field, receiver, or ambient condition settings based on current parameter settings
    generate_layout(p_data: int, nthreads: int = 0) -> bool
        Create a solar field layout
    assign_layout(p_data: int, helio_data: list, nthreads: int = 0) -> bool
        Run layout with specified positions, (optional canting and aimpoints)
    get_layout_info(p_data: int, get_corners: bool = False, restype: str = "dataframe")
        Get information regarding the heliostat field layout
    simulate(p_data: int, nthreads: int = 1, update_aimpoints: bool = True) -> bool
        Calculate heliostat field performance
    summary_results(p_data: int, save_dict: bool = True)
        Prints table of summary results from each simulation
    detail_results(p_data: int, selhel: list = None, restype: str = "dataframe", get_corners: bool = False)
        Get heliostat field layout detail results
    get_fluxmap(p_data: int, rec_id: int = 0) -> list
        Retrieve the receiver fluxmap, optionally specifying the receiver ID to retrive
    clear_land(p_data: int, clear_type: str = None) -> None
        Reset the land boundary polygons, clearing any data
    add_land(p_data: int, add_type: str, poly_points: list, is_append: bool = True) -> bool
        Add land inclusion or a land exclusion region within a specified polygon
    heliostats_by_region(p_data: int, coor_sys: str = 'all', **kwargs)
        Returns heliostats that exists within a region
    modify_heliostats(p_data: int, helio_dict: dict) -> bool
        Modify attributes of a subset of heliostats in the current layout
    save_from_script(p_data: int, sp_fname: str) -> bool
        Save the current case as a SolarPILOT .spt file
    dump_varmap_tofile(p_data: int, fname: str) -> bool
        Dump the variable structure to a text csv file
    """

    def __init__(self, debug: bool = False):
        cwd = os.getcwd()
        is_debugging = debug
        if sys.platform == 'win32' or sys.platform == 'cygwin':
            if is_debugging:
                self.pdll = CDLL(cwd + "/solarpilotd.dll")
            else:
                self.pdll = CDLL(cwd + "/solarpilot.dll")
        elif sys.platform == 'darwin':
            self.pdll = CDLL(cwd + "/solarpilot.dylib")  # Never tested
        elif sys.platform.startswith('linux'):
            self.pdll = CDLL(cwd +"/solarpilot.so")  # Never tested
        else:
            print( 'Platform not supported ', sys.platform)

    def version(self, p_data: int) -> str:
        """Provides SolarPILOT version number
        
        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance 

        Returns
        -------
        str
            SolarPILOT version number
        """

        self.pdll.sp_version.restype = c_char_p
        return self.pdll.sp_version(c_void_p(p_data) ).decode()

    def data_create(self) -> int:
        """Creates an instance of SolarPILOT in memory

        Returns
        -------
        int
            memory address of SolarPILOT instance 
        """

        self.pdll.sp_data_create.restype = c_void_p
        return self.pdll.sp_data_create()

    def data_free(self, p_data: int) -> bool:
        """Frees SolarPILOT instance from memory

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance 

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        self.pdll.sp_data_free.restype = c_bool
        return self.pdll.sp_data_free(c_void_p(p_data))

    def api_callback_create(self,p_data: int) -> None:
        """Creates a callback function for message transfer
        
        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        """

        self.pdll.sp_set_callback(c_void_p(p_data), api_callback)

    def api_disable_callback(self,p_data: int) -> None:
        """Disables callback function
                
        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        """

        self.pdll.sp_disable_callback(c_void_p(p_data))

    #SPEXPORT bool sp_set_number(sp_data_t p_data, const char* name, sp_number_t v);
    def data_set_number(self, p_data: int, name: str, value) -> bool:
        """Sets a SolarPILOT numerical variable, used for float, int, bool, and numerical combo options.

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name
        value: float, int, bool
            Desired variable value

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        self.pdll.sp_set_number.restype = c_bool
        return self.pdll.sp_set_number(c_void_p(p_data), c_char_p(name.encode()), c_number(value)) 

    #SPEXPORT bool sp_set_string(sp_data_t p_data, const char *name, const char *value)
    def data_set_string(self, p_data: int, name: str, svalue: str) -> bool:
        """Sets a SolarPILOT string variable, used for string and combos

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name
        svalue : str
            Desired variable str value

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        self.pdll.sp_set_string.restype = c_bool
        return self.pdll.sp_set_string(c_void_p(p_data), c_char_p(name.encode()), c_char_p(svalue.encode()))

    #SPEXPORT bool sp_set_array(sp_data_t p_data, const char *name, sp_number_t *pvalues, int length)
    def data_set_array(self, p_data: int, name: str, parr: list) -> bool:
        """Sets a SolarPILOT array variable, used for double and int vectors

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name
        parr : list
            Vector of data (float or int) \n

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        count = len(parr)
        arr = (c_number*count)()
        arr[:] = parr # set all at once
        self.pdll.sp_set_array.restype = c_bool
        return self.pdll.sp_set_array(c_void_p(p_data), c_char_p(name.encode()), pointer(arr), c_int(count))

    # Set array variable through a csv file
    def data_set_array_from_csv(self, p_data: int, name: str, fn: str) -> bool:
        """Sets a SolarPILOT vector variable from a csv, used for double and int vectors

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name
        fn : str
            CSV file path

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        f = open(fn, 'r', encoding="utf-8-sig")
        data = []
        for line in f:
            data.extend([n for n in map(float, line.split(','))])
        f.close()
        return self.data_set_array(p_data, name, data)

    #SPEXPORT bool sp_set_matrix(sp_data_t p_data, const char *name, sp_number_t *pvalues, int nrows, int ncols)
    def data_set_matrix(self, p_data: int, name: str, mat: list) -> bool:
        """Sets a SolarPILOT matrix variable, used for double and int matrix

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name
        mat : list of list
            Matrix of data

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        nrows = len(mat)
        ncols = len(mat[0])
        size = nrows*ncols
        arr = (c_number*size)()
        idx = 0
        for r in range(nrows):
            for c in range(ncols):
                arr[idx] = c_number(mat[r][c])
                idx += 1
        self.pdll.sp_set_matrix.restype = c_bool
        return self.pdll.sp_set_matrix( c_void_p(p_data), c_char_p(name.encode()), pointer(arr), c_int(nrows), c_int(ncols))

    # Set matrix variable values through a csv file
    def data_set_matrix_from_csv(self, p_data: int, name: str, fn: str) -> bool:
        """Sets a SolarPILOT matrix variable from a csv, used for double and int matrix

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name
        fn : str
            CSV file path

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        f = open(fn, 'r', encoding="utf-8-sig")
        data = [] 
        for line in f : 
            lst = ([n for n in map(float, line.split(','))])
            data.append(lst)
        f.close()
        return self.data_set_matrix(p_data, name, data) 

    #SPEXPORT sp_number_t sp_get_number(sp_data_t p_data, const char* name)
    def data_get_number(self, p_data: int, name: str) -> float:
        """Gets a SolarPILOT numerical variable value

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name

        Returns
        -------
        float
            Variable value 
        """

        self.pdll.sp_get_number.restype = c_number
        return self.pdll.sp_get_number(c_void_p(p_data), c_char_p(name.encode()))

    #SPEXPORT const char *sp_get_string(sp_data_t p_data, const char *name)
    def data_get_string(self, p_data: int, name: str) -> str:
        """Gets a SolarPILOT string variable value

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name

        Returns
        -------
        str
            Variable value 
        """

        self.pdll.sp_get_string.restype = c_char_p
        return self.pdll.sp_get_string(c_void_p(p_data), c_char_p(name.encode())).decode()

    #SPEXPORT sp_number_t *sp_get_array(sp_data_t p_data, const char *name, int *length)
    def data_get_array(self, p_data: int, name: str) -> list:
        """Gets a SolarPILOT array (vector) variable value

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name

        Returns
        -------
        list
            Variable value
        """

        count = c_int()
        self.pdll.sp_get_array.restype = POINTER(c_number)
        parr = self.pdll.sp_get_array(c_void_p(p_data), c_char_p(name.encode()), byref(count))
        arr = parr[0:count.value]
        return arr

    #SPEXPORT sp_number_t *sp_get_matrix(sp_data_t p_data, const char *name, int *nrows, int *ncols)
    def data_get_matrix(self,p_data: int,name: str) -> list:
        """Gets a SolarPILOT matrix variable value

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        name : str
            SolarPILOT variable name

        Returns
        -------
        list of list
            Variable value 
        """

        nrows = c_int()
        ncols = c_int()
        self.pdll.sp_get_matrix.restype = POINTER(c_number)
        parr = self.pdll.sp_get_matrix( c_void_p(p_data), c_char_p(name.encode()), byref(nrows), byref(ncols) )
        mat = []
        for r in range(nrows.value):
            row = []
            for c in range(ncols.value):
                row.append( float(parr[ncols.value * r + c]))
            mat.append(row)
        return mat

    #SPEXPORT void sp_reset_geometry(sp_data_t p_data)
    def reset_vars(self, p_data: int) -> bool:
        """Resets SolarPILOT variable values to defaults

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance

        Returns
        -------
        bool
            True if successful, False otherwise 
        """

        return self.pdll.sp_reset_geometry( c_void_p(p_data))

    #SPEXPORT int sp_add_receiver(sp_data_t p_data, const char* receiver_name)
    def add_receiver(self, p_data: int, rec_name: str) -> int:
        """Creates a receiver object

        NOTE: CoPylot starts with a default receiver configuration at receiver object ID = 0, with 'Receiver 1' as the receiver's name. 
        If you add a receiver object without dropping this default receiver, generating a layout will result in a multi-receiver problem, 
        which could produce strange results.

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        rec_name : str
            Receiver name

        Returns
        -------
        int
            Receiver object ID 
        """

        self.pdll.sp_add_receiver.restype = c_int
        return self.pdll.sp_add_receiver( c_void_p(p_data), c_char_p(rec_name.encode()))

    #SPEXPORT bool sp_drop_receiver(sp_data_t p_data, const char* receiver_name)
    def drop_receiver(self, p_data: int, rec_name: str) -> bool:
        """Deletes a receiver object

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        rec_name : str
            Receiver name

        Returns
        -------
        bool
            True if successful, False otherwise 
        """

        self.pdll.sp_drop_receiver.restype = c_bool
        return self.pdll.sp_drop_receiver( c_void_p(p_data), c_char_p(rec_name.encode()))

    #SPEXPORT int sp_add_heliostat_template(sp_data_t p_data, const char* heliostat_name)
    def add_heliostat_template(self, p_data: int, helio_name: str) -> int:
        """Creates a heliostat template object

        NOTE: CoPylot starts with a default heliostat template at ID = 0, with 'Template 1' as the Heliostat's name. 
        If you add a heliostat template object without dropping this default template, generating a layout will fail 
        because the default heliostat geometry distribution ('solarfield.0.template_rule') is 'Use single template' 
        but the select heliostat geometry ('solarfield.0.temp_which') is not defined.

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        helio_name : str
            heliostat template name

        Returns
        -------
        int
            heliostate template ID 
        """

        self.pdll.sp_add_heliostat_template.restype = c_int
        return self.pdll.sp_add_heliostat_template( c_void_p(p_data), c_char_p(helio_name.encode()))

    #SPEXPORT bool sp_drop_heliostat_template(sp_data_t p_data, const char* heliostat_name)
    def drop_heliostat_template(self, p_data: int, helio_name: str) -> bool:
        """Deletes heliostat template object

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        helio_name : str
            Heliostat template name

        Returns
        -------
        bool
            True if successful, False otherwise 
        """

        self.pdll.sp_drop_heliostat_template.restype = c_bool
        return self.pdll.sp_drop_heliostat_template( c_void_p(p_data), c_char_p(helio_name.encode()))

    #SPEXPORT bool sp_generate_simulation_days(sp_data_t p_data, int *nrecord)
    def generate_simulation_days(self, p_data: int):
        """Report out the days, hours, and weather data used to generate the annual performance estimate
        for the heliostat field layout. 

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance

        Returns
        -------
        list | Each row is a time step; columns are as follows:
            0 | Month (1-12)
            1 | Day of the month (1-N)
            2 | Hour of the day (0-23.999..)
            3 | DNI (W/m^2) direct normal irradiance
            4 | T_db (C)) dry bulb temperature
            5 | V_wind (m/s) wind velocity
            6 | Step_weight (-) relative weight given to each step during layout
        """
        nrecord = c_int()
        ncol = c_int()
        self.pdll.sp_generate_simulation_days.restype = POINTER(c_number)
        simdays = self.pdll.sp_generate_simulation_days( c_void_p(p_data), byref(nrecord), byref(ncol))
        self.pdll._sp_free_var.restype = c_void_p
        self.pdll._sp_free_var( byref(simdays) )
        data = []
        for i in range(nrecord.value):
            data.append(simdays[i*ncol.value:i*ncol.value+ncol.value])

        return data


    #SPEXPORT bool sp_update_geometry(sp_data_t p_data)
    def update_geometry(self, p_data: int) -> bool:
        """Refresh the solar field, receiver, or ambient condition settings based on current parameter settings

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance

        Returns
        -------
        bool
            True if successful, False otherwise 
        """

        self.pdll.sp_update_geometry.restype = c_bool
        return self.pdll.sp_update_geometry( c_void_p(p_data))

    #SPEXPORT bool sp_generate_layout(sp_data_t p_data, int nthreads = 0)
    def generate_layout(self, p_data: int, nthreads: int = 0) -> bool:
        """Create a solar field layout

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        nthreads : int, optional
            Number of threads to use for simulation

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        self.pdll.sp_generate_layout.restype = c_bool
        return self.pdll.sp_generate_layout( c_void_p(p_data), c_int(nthreads))

    #SPEXPORT bool sp_assign_layout(sp_data_t p_data, sp_number_t* pvalues, int nrows, int ncols, int nthreads = 0) //, bool save_detail = true)
    def assign_layout(self, p_data: int, helio_data: list, nthreads: int = 0) -> bool:
        """Run layout with specified positions, (optional canting and aimpoints) 

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        helio_data : list of lists
                heliostat data to assign
                [<template id (int)>, <location X>, <location Y>, <location Z>], <x focal length>, <y focal length>, <cant i>, <cant j>, <cant k>, <aim X>, <aim Y>, <aim Z>]
                NOTE: First 4 columns are required, the rest are optional.
        nthreads : int, optional
            Number of threads to use for simulation

        Returns
        -------
        bool
            True if successful, False otherwise
        """

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
    def get_layout_info(self, p_data: int, get_corners: bool = False, restype: str = "dataframe"):
        """Get information regarding the heliostat field layout

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        get_corner : bool, optional
            True, output will include heliostat corner infromation, False otherwise
        restype : str, optional 
            result format type, supported options: "matrix", "dictionary", "dataframe"

        Returns
        -------
        pandas.DataFrame()
            heliostat field layout infromation in dataframe format
        dict
            heliostat field layout infromation in dictionary format
        list of list (matrix), list of strings
            heliostat field layout infromation with data in a matrix and column names in a header list of strings
        """

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
    def simulate(self, p_data: int, nthreads: int = 1, update_aimpoints: bool = True) -> bool:
        """Calculate heliostat field performance

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        nthreads : int, optional
            number of threads to use for simulation
        update_aimpoints : bool, optional
            True, aimpoints update during simulation, False otherwise 

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        self.pdll.sp_simulate.restype = c_bool
        return self.pdll.sp_simulate( c_void_p(p_data), c_int(nthreads), c_bool(update_aimpoints))

    #SPEXPORT const char *sp_summary_results(sp_data_t p_data)
    def summary_results(self, p_data: int, save_dict: bool = True):
        """Prints table of summary results from each simulation

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        save_dict : bool, optional
            True, return results as dictionary

        Returns
        -------
        dict
            dictionary containing simulation summary results (default)
        None
            prints summary table to terminal (save_dict = False)
        """

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
    def detail_results(self, p_data: int, selhel: list = None, restype: str = "dataframe", get_corners: bool = False):
        """Get heliostat field layout detail results

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        selhel : list, optional
            Selected heliostat ID
        restype : str, optional
            result format type, supported options "matrix", "dictionary", "dataframe" \n
        get_corner : bool, optional
            True, output will include heliostat corner infromation, False otherwise

        Returns
        -------
        pandas.DataFrame()
            detailed heliostat field results in dataframe format
        dict
            detailed heliostat field results in dictionary format
        list of list (matrix), list of strings
            detailed heliostat field results with data in a matrix and column names in a header list of strings
        """

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
            print("detail_results API called failed to return correct information.")

    #SPEXPORT sp_number_t* sp_get_fluxmap(sp_data_t p_data, int* nrows, int* ncols, int rec_id = 0)
    def get_fluxmap(self, p_data: int, rec_id: int = 0) -> list:
        """Retrieve the receiver fluxmap, optionally specifying the receiver ID to retrieve

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        rec_id : int, optional
            receiver ID to retrieve

        Returns
        -------
        list of lists (matrix)
            receiver fluxmap
        """

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
    def clear_land(self, p_data: int, clear_type: str = None) -> None:
        """Reset the land boundary polygons, clearing any data

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        clear_type : str, optional
            specify land boundaries to clear, options are 'None' (default), 'inclusion', or 'exclusion'

        Returns
        -------
        None
        """

        self.pdll.sp_clear_land( c_void_p(p_data), c_char_p(clear_type.encode()))

    #SPEXPORT bool sp_add_land(sp_data_t p_data, const char* type, sp_number_t* polygon_points, int* npts , int* ndim, bool is_append = true)
    def add_land(self, p_data: int, add_type: str, poly_points: list, is_append: bool = True) -> bool:
        """Add land inclusion or a land exclusion region within a specified polygon

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        add_type : str
            specify type of added land boundary, options are inclusion' or 'exclusion'
        poly_points : list of lists (matrix)
            list of polygon points [[x1,y1],[x2,y2],...] or [[x1,y1,z1],[x2,y2,z2],...]
        is_append : bool, optional
            Append (True) or overwrite (False) the existing regions

        Returns
        -------
        bool
            True if successful, False otherwise
        """

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
        return self.pdll.sp_add_land( c_void_p(p_data), c_char_p(add_type.encode()), pointer(PParr), byref(c_int(npts)), byref(c_int(ndim)), c_bool(is_append))

    #SPEXPORT sp_number_t* sp_heliostats_by_region(sp_data_t p_data, const char* coor_sys, int* lenret,
    #                                                sp_number_t* arguments = NULL, int* len_arg = NULL,
    #                                                const char* svgfname_data = NULL, sp_number_t* svg_opt_tab = NULL);
    def heliostats_by_region(self, p_data: int, coor_sys: str = 'all', **kwargs):
        """Returns heliostats that exists within a region

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        coor_sys : str, optional
            Options are
                'all' (no additional infromation required),
                'cylindrical' (provide 'arguments' [rmin, rmax, azmin radians, azmax radians]),
                'cartesian' (provide 'arguments' [xmin, xmax, ymin, ymax[, zmin, zmax]]),
                'polygon' (provide 'arguments' [[x1,y1],[x2,y2],...]),
                'svg' (provide 'svgfname_data' string with 'scale-x scale-y;offset-x offset-y;<svg path 1>;<svg path 2>;...',
                'svgfile' (provide 'svgfname_data' string of filename with path, optional 'svg_opt_tab' table (list) [scale-x, scale-y, offset-x, offset-y])

        kwargs
        ------
        arguments : list
            depends on coor_sys selected
        svgfname_data : str
            either svg data or a svg file path
        svg_opt_tab : list, optional
            svg optional table for scale and offset [x-scale, y-scale, x-offset, y-offset]
        restype : str, optional 
            result format type, supported options: "matrix", "dictionary", "dataframe"

        Returns
        -------
        pandas.DataFrame()
            heliostat field layout infromation in dataframe format (default)
        dict
            heliostat field layout infromation in dictionary format
        list of list (matrix), list of strings
            heliostat field layout infromation with data in a matrix and column names in a header list of strings
        """

        argsdict = {
            'arguments': [],
            'svgfname_data': '', 
            'svg_opt_tab' : [],
            'restype': 'dataframe'
        }
        argsdict.update(kwargs)

        # flatten polygon points
        if coor_sys == 'polygon':
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
        if coor_sys == 'all':
            res = self.pdll.sp_heliostats_by_region( c_void_p(p_data), c_char_p(coor_sys.encode()), byref(lenret) )
        elif coor_sys == 'cylindrical' or coor_sys == 'cartesian' or coor_sys == 'polygon':
            res = self.pdll.sp_heliostats_by_region( c_void_p(p_data), c_char_p(coor_sys.encode()), byref(lenret), pointer(arg_arr), byref(c_int(len_arg)))
        elif coor_sys == 'svg' or coor_sys == 'svgfile':
            if len(argsdict['svg_opt_tab'] == 0):
                res = self.pdll.sp_heliostats_by_region( c_void_p(p_data), c_char_p(coor_sys.encode()), byref(lenret), pointer(arg_arr), byref(c_int(len_arg)), c_char_p(argsdict['svgfname_data'].encode()) )
            elif len(argsdict['svg_opt_tab']) == 4:
                svg_tab = (c_number*4)()
                svg_tab[:] = argsdict['svg_opt_tab']
                res = self.pdll.sp_heliostats_by_region( c_void_p(p_data), c_char_p(coor_sys.encode()), byref(lenret), pointer(arg_arr), byref(c_int(len_arg)), c_char_p(argsdict['svgfname_data'].encode()), pointer(svg_tab) )
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
    def modify_heliostats(self, p_data: int, helio_dict: dict) -> bool:
        """Modify attributes of a subset of heliostats in the current layout

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        helio_dict : dict
            Heliostat modified attributes, dictionary keys are as follows:
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

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        ncols = len(helio_dict.keys())
        nhel = len(helio_dict[next(iter(helio_dict))])
        table_hdr = ""
        for key in helio_dict.keys():
            table_hdr += key
            if (key != list(helio_dict)[-1]):
                table_hdr += ','
        
        size = ncols*nhel
        helio_data = (c_number*size)()
        idx = 0
        for h in range(nhel):
            for key in helio_dict.keys():
                helio_data[idx] = c_number(helio_dict[key][h])
                idx += 1

        self.pdll.sp_modify_heliostats.restype = c_bool
        return self.pdll.sp_modify_heliostats( c_void_p(p_data), pointer(helio_data), byref(c_int(nhel)), byref(c_int(ncols)), c_char_p(table_hdr.encode()) )

    #SPEXPORT bool sp_calculate_optical_efficiency_table(sp_data_t p_data, int ud_n_az, int ud_n_zen);
    def calculate_optical_efficiency_table(self, p_data: int,  ud_n_az: int = 0, ud_n_zen: int = 0) -> bool:
        """Calculates optical efficiency table  based an even spacing of azimuths and zeniths (elevation) angles.

         Parameters
         ----------
         p_data : int
             memory address of SolarPILOT instance
         ud_n_az : int
             user-defined number of azimuth sampling points
         ud_n_zen : int
             user-defined number of zenith (elevation) sampling points

         Returns
         -------
         bool
             True if successful, False otherwise
         """

        self.pdll.sp_calculate_optical_efficiency_table.restype = c_bool
        if ud_n_az == 0 and ud_n_zen == 0:
            return self.pdll.sp_calculate_optical_efficiency_table( c_void_p(p_data))
        else:
            return self.pdll.sp_calculate_optical_efficiency_table( c_void_p(p_data), c_int(ud_n_az), c_int(ud_n_zen))

    #SPEXPORT sp_number_t* sp_get_optical_efficiency_table(sp_data_t p_data, int* nrows, int* ncols)
    def get_optical_efficiency_table(self, p_data: int) -> dict:
        """Retrieve the field optical efficiency table as a function of azimuth and elevation angles

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        
        Returns
        -------
        dictionary with the following keys:

            #. ``azimuth``: list, Solar azimuth angle [deg]
            #. ``elevation``: list, Solar elevation angle [deg]
            #. ``eff_data``: list of lists, Solar field optical efficiency at a specific azimuth (rows) and elevation (cols) angles [-]
        """

        nrows = c_int()
        ncols = c_int()
        self.pdll.sp_get_optical_efficiency_table.restype = POINTER(c_number)
        res = self.pdll.sp_get_optical_efficiency_table( c_void_p(p_data), byref(nrows), byref(ncols))
        # Formatting output
        elevation = []
        azimuth = []
        eff_data = []
        for r in range(nrows.value):
            row = []
            for c in range(ncols.value):
                if r == 0 and c == 0:
                    pass
                elif r == 0:
                    elevation.append( float(res[ncols.value * r + c]))
                elif r != 0 and c == 0:
                    azimuth.append( float(res[ncols.value * r + c]))
                else:
                    row.append( float(res[ncols.value * r + c]))
            if r != 0:
                eff_data.append(row)
        
        return {'azimuth': azimuth, 'elevation': elevation, 'eff_data': eff_data}
    

    #SPEXPORT bool sp_save_optical_efficiency_table(sp_data_t p_data, const char* sp_fname, const char* table_name)
    def save_optical_efficiency_table(self, p_data: int, sp_fname: str, modelica_table_name: str = 'none') -> bool:
        """
        Saves optical efficiency table as a CSV file in the following format:
            First row: Elevation angles (with leading zero)
            First column: Azimuth angles
            Rest of columns: Optical efficiency corresponding to elevation angle       

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        sp_fname : str
            filename to save efficiency table
        modelica_table_name : str (optional)
            Modelica table name for table output file consistent with Modelica formatting requirements. 
            If not provided, then table format follows get_optical_efficiency_table(). 
            Otherwise, table format contains extra header lines and elevation angle to be in ascending order (required by Modelica).

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        self.pdll.sp_save_optical_efficiency_table.restype = c_bool
        return self.pdll.sp_save_optical_efficiency_table( c_void_p(p_data), c_char_p(sp_fname.encode()), c_char_p(modelica_table_name.encode()))

    #SPEXPORT bool sp_save_from_script(sp_data_t p_data, const char* sp_fname)
    def save_from_script(self, p_data: int, sp_fname: str) -> bool:
        """Save the current case as a SolarPILOT .spt file

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        sp_fname : str
            filename to save SolarPILOT case

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        self.pdll.sp_save_from_script.restype = c_bool
        return self.pdll.sp_save_from_script( c_void_p(p_data), c_char_p(sp_fname.encode()))

    #SPEXPORT bool sp_load_from_script(sp_data_t p_data, const char* sp_fname)
    def load_from_script(self, p_data: int,  sp_fname: str) -> bool:
        """Load a SolarPILOT .spt file

         Parameters
         ----------
         p_data : int
             memory address of SolarPILOT instance
         sp_fname : str
             filename to load SolarPILOT case

         Returns
         -------
         bool
             True if successful, False otherwise
         """

        self.pdll.sp_load_from_script.restype = c_bool
        return self.pdll.sp_load_from_script( c_void_p(p_data), c_char_p(sp_fname.encode()))

    #SPEXPORT bool sp_dump_varmap(sp_data_t p_data, const char* sp_fname)
    def dump_varmap_tofile(self, p_data: int, fname: str) -> bool:
        """Dump the variable structure to a text csv file

        Parameters
        ----------
        p_data : int
            memory address of SolarPILOT instance
        fname : str
            filename to save variables

        Returns
        -------
        bool
            True if successful, False otherwise
        """

        self.pdll.sp_dump_varmap.restype = c_bool
        return self.pdll.sp_dump_varmap( c_void_p(p_data), c_char_p(fname.encode()))
    
