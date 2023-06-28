#!/usr/bin/env python3

#  Python Libraries
import argparse
import datetime
import re

import numpy as np

#  Plotly Libraries
import plotly.graph_objects as go
from plotly.subplots import make_subplots

#  Pandas Libraries
import pandas as pd


class rule_base:

    def get_headers(self):
        raise NotImplementedError('Not implemented yet')

    @staticmethod
    def get_key_value( log_entry, key_name ):
        idx = log_entry.find( key_name )
        if idx < 0:
            return None

        parts = log_entry[(idx + len(key_name)):].strip().split()
        return parts[0]

    @staticmethod
    def get_key_value_cast( entry, key, dtype ):

        value = rule_base.get_key_value( entry, key )
        if value is None:
            return None
        else:
            return dtype(value)

    @staticmethod
    def get_timestamp( line ):

        #  get the portion with the date and time
        parts = line.split()
        date_str = parts[0][1:]
        time_str = parts[1][0:-1]

        return datetime.datetime.strptime( date_str + ' ' + time_str,
                                           '%Y-%m-%d %H:%M:%S.%f' )


#  Stick your fancy rules here.  Eventually, when I have time, I'll
#  add configurable items to this.
class rule_pattern_with_stats_entry(rule_base):

    def get_headers(self):
        return [ 'mean','min','max','count',
                 'rolling_mean',
                 'stddev',
                 'rolling_stddev',
                 'timestamp' ]

    @staticmethod
    def is_valid( log_entry ):

        result = False
        if 'Adding Entry' in log_entry:
            result = True
        return result

    def parse( self, log_entry ):

        return { 'timestamp':      self.get_timestamp( log_entry ),
                 'mean':           self.get_key_value_cast( log_entry,   ' MEAN ', float ),
                 'min' :           self.get_key_value_cast( log_entry,    ' MIN ', float ),
                 'max' :           self.get_key_value_cast( log_entry,          ' MAX ', float ),
                 'rolling_mean':   self.get_key_value_cast( log_entry, ' ROLLING_MEAN ', float ),
                 'stddev':         self.get_key_value_cast( log_entry, ' STDDEV ', float ),
                 'rolling_stddev': self.get_key_value_cast( log_entry, ' ROLLING_STDDEV ', float ),
                 'count':          self.get_key_value_cast( log_entry, ' COUNT ', int ) }


#  Register all rules here
rule_set = [ rule_pattern_with_stats_entry() ]

def filter_entries( entries ):

    column_data = {}
    for rule in rule_set:
        for header in rule.get_headers():
            column_data[header] = []

    for entry in entries:

        # parse line to see if it's a full entry
        new_entry = { 'update': False }
        for rule in rule_set:
            if rule.is_valid( entry ):
                result = rule.parse( entry )
                for key in result:
                    new_entry[key] = result[key]
                    new_entry['update'] = True

        # if good to update, add to columns
        if new_entry['update']:
            for column_tag in column_data:
                if column_tag in new_entry:
                    column_data[column_tag].append( new_entry[column_tag] )
                else:
                    column_data[column_tag].append( None )


    df = pd.DataFrame( data = column_data )
    return df

# Looking for something like:  [2023-06-21 11:12:27.402101] [0x00007ff8506f6640] [info] <msg>
new_entry_pattern = re.compile( '\[[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\.\d+\].*$')

def Is_Start_New_Log_Entry( log_entry ):
    '''
    Method checks if the log entry is the start of a new entry or
    a continuation of the previous
    '''
    return ( re.match( new_entry_pattern, log_entry ) != None)



def plot( cmd_options, log_data ):

    fig = make_subplots(rows=2, cols=1, shared_xaxes=True )

    #  Min entry
    fig.add_trace( go.Scatter( x=log_data['timestamp'], y=log_data['min'],
                               fill=None,
                               mode='lines',
                               line_color='indigo',
                                name = 'min' ),
                   row = 1,
                   col = 1
                 )


    # Mean (Line)
    fig.add_trace(go.Scatter(x=log_data['timestamp'], y=log_data['mean'],
        fill=None,
        mode='lines',
        line_color='black',
        name = 'mean' ),
        row = 1,
        col = 1
        )

    fig.add_trace(go.Scatter(x=log_data['timestamp'], y=( log_data['mean'] + 2 * log_data['stddev'] ),
        fill=None,
        mode='lines',
        line_color='orange',
        name = 'upper_quantile'),
        row = 1,
        col = 1
        )

    fig.add_trace(go.Scatter(x=log_data['timestamp'], y=log_data['stddev'],
        fill=None,
        mode='lines',
        line_color='red',
        name = 'stddev'),
        row = 1,
        col = 1
        )

    fig.add_trace(go.Scatter(x=log_data['timestamp'], y=log_data['rolling_stddev'],
        fill=None,
        mode='lines',
        line_color='green',
        name = 'rolling stddev' ),
        row = 1,
        col = 1
        )

    # Rolling Mean (Line)
    fig.add_trace(go.Scatter(x=log_data['timestamp'], y=log_data['rolling_mean'],
        fill=None,
        mode='lines',
        line_color='blue',
        name = 'rolling mean' ),
        row = 1,
        col = 1
        )

    # Count
    fig.add_trace(go.Scatter(x=log_data['timestamp'], y=log_data['count'],
        fill=None,
        mode='lines',
        line_color='blue',
        name = 'count' ),
        row = 2,
        col = 1
        )

    #  Max Entry
    #fig.add_trace(go.Scatter(x=log_data['timestamp'], y=log_data['max'],
    #    mode='lines',
    #    line_color='indigo',
    #    name = 'max' ))

    fig.update_layout( title = 'Address Book Performance' )

    fig.update_xaxes(title_text="Log Timestamp", row = 1, col = 1)
    fig.update_xaxes(title_text="Log Timestamp", row = 2, col = 1)
    fig.update_yaxes(title_text="Elapsed Time Adding Entries (ms)", row = 1, col = 1)
    fig.update_yaxes(title_text="Number of entries in address book", row = 2, col = 1)

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

    #  Plot the data
    plot( cmd_options,
          log_data )

