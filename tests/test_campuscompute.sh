#!/bin/bash

echo "Testing Job Management and CPU Scheduling"

printf '1\n1\n0\n5\n2\n1\n2\n0\n3\n1\n1\n3\n0\n1\n3\n2\n3\n4\n2\n5\n11\n' | ./build/campuscompute

echo "All Tests Completed"