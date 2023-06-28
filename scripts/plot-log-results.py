#!/usr/bin/env python3

#  Python Libraries
import argparse
import re

#  Plotly Libraries
import plotly.graph_objects as go

#  Pandas Libraries
import pandas as pd


class rule_base:

    def get_timestamp( line ):

        #  get the portion with the date and time
        parts = line.split()
        date_str = parts[0][1:]
        time_str = parts[1][0:-1]



#  Stick your fancy rules here.  Eventually, when I have time, I'll
#  add configurable items to this.
class rule_pattern_with_stats_entry(rule_base):

    def get_headers():
        return ['mean','min','max','stddev','timestamp']

    def is_valid( log_entry ):

        result = False
        if 'Adding Entry' in log_entry:
            result = True
        return result

    def parse( log_entry ):

        timestamp = get_timestamp( log_entry )
        mean_entry   = 0
        min_entry    = 0
        max_entry    = 0
        stddev_entry = 0
        return { 'timestamp': timestamp,
                 'mean':      mean_entry,
                 'min' :      min_entry,
                 'max' :      max_entry,
                 'stddev':    stddev_entry }


#  Register all rules here
rule_set = [ rule_pattern_with_stats_entry() ]

def filter_entries( entries ):

    column_list = []
    for rule in rule_set:
        column_list.append( rule.get_headers() )

    df = pd.DataFrame( columns = column_list )

    for entry in entries:
        for rule in rule_set:
            if rule.is_valid( rule ):

    return df

# Looking for something like:  [2023-06-21 11:12:27.402101] [0x00007ff8506f6640] [info] <msg>
new_entry_pattern = re.compile( '\[[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\.\d+\].*$')

def Is_Start_New_Log_Entry( log_entry ):
    '''
    Method checks if the log entry is the start of a new entry or
    a continuation of the previous
    '''
    return ( re.match( new_entry_pattern, log_entry ) != None)



def Plot():
    fig = go.Figure()
    fig.add_trace(go.Scatter(x=[1, 2, 3, 4], y=[3, 4, 8, 3],
        fill=None,
        mode='lines',
        line_color='indigo',
        ))
    fig.add_trace(go.Scatter(
        x=[1, 2, 3, 4],
        y=[1, 6, 2, 6],
        fill='tonexty', # fill area between trace0 and trace1
        mode='lines', line_color='indigo'))

    fig.show()

def parse_config_file():

    parser = argparse.ArgumentParser( description='Plot components from log file' )

    parser.add_argument( '-l',
                          dest='log_path',
                          required=True,
                          help='Path to log file.' )

    return parser.parse_args()

def parse_log_file( cmd_options ):

    log_entries = []

    #  open the log file
    with open( cmd_options.log_path, 'r' ) as fin:

        current_entry = ''

        for line in fin.readlines():

            #  Concat the lines until you get to a new entry
            is_new_entry = Is_Start_New_Log_Entry( line )
            if is_new_entry:
                if line != None:
                    current_entry = current_entry.strip(' \n')
                    if len(current_entry) > 0:
                        log_entries.append( current_entry )
                current_entry = line
            else:
                current_entry += ' ' + line

    return log_entries

if __name__ == '__main__':

    #  Parse command-line options
    cmd_options = parse_config_file()

    #  Parse the log file
    entries = parse_log_file( cmd_options )

    #  Apply the rules to the log file
    #  Note:  I should apply the rules during parsing of the log file, but
    #         I want to make these steps as isolated and clean as possible.
    log_data = filter_entries( entries )


