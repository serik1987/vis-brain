#-*- coding: utf-8

import os
import re
import datetime
_simulation_name_template = re.compile("^([A-Za-z0-9-]+)_(\d+)_(\d{4})(\d{2})(\d{2})")


def get_project_folders():
    '''
    Lists all simulation folders in the current directory

    :return: list of the data about simulations arranged in ascending order. Each list item is a dictionary
    with the following keys:
    'dir_name' name of the simulation folder to access
    'simulation_name' name of the simulation itself
    'simulation_number' the simulation number
    'simulation_date' date where the simulation process started
    '''
    folder_list = []
    for filename in os.listdir("."):
        if os.path.isdir(filename):
            matches = _simulation_name_template.search(filename)
            if matches is not None:
                sim_number = int(matches.group(2))
                sim_year = int(matches.group(3))
                sim_month = int(matches.group(4))
                sim_day = int(matches.group(5))
                folder_list.append({
                    "dir_name": matches.group(0),
                    "sim_name": matches.group(1),
                    "sim_number": sim_number,
                    "sim_date": datetime.date(sim_year, sim_month, sim_day)
                })
    folder_list = sorted(folder_list, key=lambda item: item["sim_number"])
    return folder_list
