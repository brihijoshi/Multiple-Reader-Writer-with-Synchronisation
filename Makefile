# CSE231: Operating Systems
# Assignment 3, Part 1: Multiple reader writer with synchronization
# Brihi Joshi (2016142)
# Taejas Gupta (2016204)
# April 12, 2018



all:
	gcc sync.c -o sync -lpthread

clean:
	rm sync