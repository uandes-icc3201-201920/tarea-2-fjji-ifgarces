import matplotlib.pyplot as plt
from numpy import array

# fifo
"""
npages  = [50, 50, 80, 80]
nframes = [30, 50, 50, 70]
pagefaults_count  = [120, 100, 190, 170]
pagereplace_count = [70,  50,  110, 90]

plt.plot(npages, pagefaults_count)
plt.plot(npages, pagereplace_count)
plt.legend(["page_faults", "page_replaces"])
plt.title("(FIFO) npages")
plt.show()

plt.plot(nframes, pagefaults_count)
plt.plot(nframes, pagereplace_count)
plt.legend(["page_faults", "page_replaces"])
plt.title("(FIFO) nframes")
plt.show()
"""


# rand
npages  = [50, 50, 80, 80]
nframes = [30, 50, 50, 70]
pagefaults_count  = [100, 100, 160, 160]
pagereplace_count = [50,  50,  80,  80]

plt.plot(npages, pagefaults_count)
plt.plot(npages, pagereplace_count)
plt.legend(["page_faults", "page_replaces"])
plt.title("(FIFO) npages")
plt.show()

plt.plot(nframes, pagefaults_count)
plt.plot(nframes, pagereplace_count)
plt.legend(["page_faults", "page_replaces"])
plt.title("(FIFO) nframes")
plt.show()