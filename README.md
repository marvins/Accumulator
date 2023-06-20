lib-accumulator : Statistical and Metrics Utility API
=============================================

lib-accumulator is designed to provide a general-purpose API for tracking metrics.  It expands upon the
features of Boost, specifically `accumulator` and `mpl`, to provide easy-to-use classes for doing the following...


Use Cases
---------


### Performance Testing

* Create a Stopwatch object to track timing info or provide your own interfaces
    * Method will help find timing of a method over time.

### Measuring Compression Ratios or other "Live" metrics

* Add results into the stats aggregator and verify the results are not acting weird over time.


Accumulator API
---------------

### Stopwatch

Provides simple API for tracking the duration of an event.  2 lines are required (Creation of object or `start()`, followed by `stop()`).  `stop()` returns a `std::chrono::duration` in the form of `milliseconds` or whatever template parameter is specified.

### Accumulator

Wrapper around `boost::accumulators::accumulator_set` which stores and prints various statistical metrics for a given dataset.  Just insert into the `Accumulator` object and it'll do the math for you.


