# CommunityDetection
Cache efficient version of the Louvain community detection algorithm. 

The folder "Louvian_updated" is the original code for the Louvain algorithm taken from the developers webpage at https://sites.google.com/site/findcommunities/. (Reference: "V. D. Blondel, J.-L. Guillaume, R. Lambiotte, and E. Lefebvre, _Fast
unfolding of communities in large networks,_ Journal of statistical mechanics: theory and experiment, vol. 2008, no. 10, p. P10008, 2008.").

In the folder "MoveFastLouvain", I have implemented a version of the MoveFast scheme from the paper "V. A. Traag, L. Waltman, and N. J. van Eck, _From louvain to leiden: guaranteeing well-connected communities,_ Scientific reports, vol. 9,2019."

The floder "graphReordering-master" is taken from https://github.com/kartiklakhotia/graphReordering (Reference: "J. Arai, H. Shiokawa, T. Yamamuro, M. Onizuka, and S. Iwamura, _Rabbit order: Just-in-time parallel reordering for fast graph analysis,_ in 2016 IEEE International Parallel and Distributed Processing Symposium (IPDPS). IEEE, 2016, pp. 22–31"); and the folder "Gorder-master" is taken from https://github.com/datourat/Gorde  (Reference: "H. Wei, J. X. Yu, C. Lu, and X. Lin, _Speedup graph processing by graph ordering,_ in Proceedings of the 2016 International Conference on Management of Data. ACM, 2016, pp. 1813–1828"). These codes are used as a preordering codes and compared against our localRCM preordering code that is more suited to the blocking done in our cache efficient version of the Louvain algorithm. Also, the folder "simpleBFS_RCM" contains the RCM implementation from Gorder-master with slight modifications which are commented by my first name (sanaz). 

All the other folders contain my additions and modifications to the functions in the folders above. My modifications and the functions I have added are
commented by my first name (sanaz). The descriptions of each folder is included in its corresponding readme file. 

The bash scripts in this parent folder are used for the purpose of running different versions of the codes together and comparing the results. They should be used when the corresponding codes are compiled. Here is a high level description of them: 

PreOrder.sh: runs different preordering methods on the real networks

snPreOrder.sh: the same as PreOrder.sh but for synthetic networks (they don't require a random shuffle). 

bulkIterativeRun.sh: performs iterative runs on a group of functions given the input (preordered) files. Here, "iterative" means that a community detection algorithm runs to completion and then a next iteration runs on the output of the previous iteration. 

nBulkIterativeRun.sh: runs the iterative version of each code on each preordered networks "n times" and reports the average runtimes and modularities. 

bulkRun.sh: runs the non-iterative version of each code on each preordered input. 
