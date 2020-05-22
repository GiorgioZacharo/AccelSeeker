#!/bin/bash

# Prepare the AccelSeeker Candidates for the selection taken into account the overlapping rule.
# Generate AccelSeeker Candidates list with Merit/Cost and Call Function Indexes.

printf "Compute Merit/Cost File - MC.txt"
../scripts/compute_merit.sh

printf "Generate Overlapping Rule - FCI_CROPPED_OVERLAP_RULE.txt"
../scripts/prune_fci_files.sh
python ../scripts/generate_overlapping_rule.py 

printf "Merge MC.txt and FCI_CROPPED_OVERLAP_RULE.txt"
printf "Generate AccelSeeker Candidates list with Merit/Cost and Call Function Indexes - MCI.txt"
../scripts/merge_mc_and_fci.sh
