#!/bin/bash

# Where to store everything
OUTPUT_DIR="./sim_results"
mkdir -p "$OUTPUT_DIR"

# Simulation parameters
CHANNEL_MODEL="ThreeGpp"
CHANNEL_CONDITION="NLOS"

# Loop settings
START_SEED=100
END_SEED=110
RUN_PER_SEED=3

# Files to collect
TRACE_FILES=("NrDlMacStats.txt" "DlDataSinr.txt" "RxedGnbMacCtrlMsgsTrace.txt" "hexagonal-topology.gnuplot")

# Simulation command
NS3_BIN="./ns3"

# Looping
for (( SEED=$START_SEED; SEED<$END_SEED; SEED++ )); do
  for (( RUN=1; RUN<=$RUN_PER_SEED; RUN++ )); do
    echo ">>> Running SEED=$SEED, RUN=$RUN"

    # Run simulation
    $NS3_BIN run "scratch/opt-gsoc-nr-channel-models-error \
      --channelModel=$CHANNEL_MODEL \
      --channelConditionModel=$CHANNEL_CONDITION \
      --seed=$SEED \
      --run=$RUN"

    # Create folder for this run
    RUN_FOLDER="$OUTPUT_DIR/seed${SEED}_run${RUN}"
    mkdir -p "$RUN_FOLDER"

    # Move desired trace files
    for TRACE_FILE in "${TRACE_FILES[@]}"; do
      if [ -f "$TRACE_FILE" ]; then
        mv "$TRACE_FILE" "$RUN_FOLDER/"
        echo "  ✓ Moved $TRACE_FILE to $RUN_FOLDER/"
      else
        echo "  ⚠️  Warning: $TRACE_FILE not found"
      fi
    done
  done
done
