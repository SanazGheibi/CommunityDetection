# CommunityDetection
Cache efficient version of the Louvain community detection algorithm. 

The folder "Louvian_updated" is the original code for the Louvain algorithm taken from the developers webpage at https://sites.google.com/site/findcommunities/

In the folder "MoveFastLouvain", I have implemented a version of the MoveFast scheme from the paper "V. A. Traag, L. Waltman, and N. J. van Eck, “From louvain to leiden:
guaranteeing well-connected communities,” Scientific reports, vol. 9,2019."

The floder "graphReordering-master" is taken from https://github.com/kartiklakhotia/graphReordering. This code is used as a preordering code and compared against
our localRCM preordering code that is more suited to the blocking done in our cache efficient version of the Louvain algorithm. Also, the folder 
"simpleBFS_RCM" contains the RCM implementation from graphReordering-master with slight modifications which are commented by my first name (sanaz). 

All the other folders contain my additions and modifications to the functions in the folders above. My modifications and the functions I have added are
commented by my first name (sanaz). 
