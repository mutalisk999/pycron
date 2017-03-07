import sys, time, pycron

print pycron.last_cronexpr_time("0 * * * * ?", int(time.time()))
print pycron.next_cronexpr_time("0 * * * * ?", int(time.time()))

print pycron.last_cronexpr_time("0/5 * * * * ?", int(time.time()))
print pycron.next_cronexpr_time("0/5 * * * * ?", int(time.time()))

print pycron.last_cronexpr_time("1,2,3 * * * * ?", int(time.time()))
print pycron.next_cronexpr_time("1,2,3 * * * * ?", int(time.time()))


'''
1478964180
1478964240
1478964235
1478964240
1478964183
1478964241
'''
