FIND_PROGRAM(PY_COMMAND $ENV{PYTHON_ROOT}/bin/python)

EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/air.py             ${CMAKE_CURRENT_BINARY_DIR}/air.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/atomicdata.py      ${CMAKE_CURRENT_BINARY_DIR}/atomicdata.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/glue.py            ${CMAKE_CURRENT_BINARY_DIR}/glue.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/pvc.py             ${CMAKE_CURRENT_BINARY_DIR}/pvc.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/soup.py            ${CMAKE_CURRENT_BINARY_DIR}/soup.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/util.py            ${CMAKE_CURRENT_BINARY_DIR}/util.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/scint.py           ${CMAKE_CURRENT_BINARY_DIR}/scint.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ndrock.py          ${CMAKE_CURRENT_BINARY_DIR}/ndrock.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/concrete.py        ${CMAKE_CURRENT_BINARY_DIR}/concrete.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/printscintfd.py    ${CMAKE_CURRENT_BINARY_DIR}/printscintfd.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/printscintlate.py  ${CMAKE_CURRENT_BINARY_DIR}/printscintearly.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/printscintearly.py ${CMAKE_CURRENT_BINARY_DIR}/printscintlate.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/printglue.py       ${CMAKE_CURRENT_BINARY_DIR}/printglue.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/printpvc.py        ${CMAKE_CURRENT_BINARY_DIR}/printpvc.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/printair.py        ${CMAKE_CURRENT_BINARY_DIR}/printair.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/printndrock.py     ${CMAKE_CURRENT_BINARY_DIR}/printndrock.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/printconcrete.py   ${CMAKE_CURRENT_BINARY_DIR}/printconcrete.py)


EXECUTE_PROCESS(COMMAND ${PY_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/printscintfd.py    OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/scintfd.gdml)
EXECUTE_PROCESS(COMMAND ${PY_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/printscintlate.py  OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/scintlate.gdml)
EXECUTE_PROCESS(COMMAND ${PY_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/printscintearly.py OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/scintearly.gdml)
EXECUTE_PROCESS(COMMAND ${PY_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/printglue.py       OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/glue.gdml)
EXECUTE_PROCESS(COMMAND ${PY_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/printair.py        OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/air.gdml)
EXECUTE_PROCESS(COMMAND ${PY_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/printpvc.py        OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/pvc.gdml)
EXECUTE_PROCESS(COMMAND ${PY_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/printndrock.py     OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/ndrock.gdml)
EXECUTE_PROCESS(COMMAND ${PY_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/printconcrete.py   OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/concrete.gdml)

EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/air.py            )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/atomicdata.py     )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/glue.py           )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/pvc.py            )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/soup.py           )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/util.py           )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/scint.py          )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/ndrock.py         )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/concrete.py       )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/printscintfd.py   )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/printscintlate.py )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/printscintearly.py)
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/printglue.py      )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/printpvc.py       )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/printair.py       )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/printndrock.py    )
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/printconcrete.py  )
