execute_process (
  COMMAND sed -e "/^#/d" ${IN_FILE}
  OUTPUT_FILE ${OUT_FILE}
)
