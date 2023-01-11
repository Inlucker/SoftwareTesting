from tables.check.delete import CheckDelete
from tables.check.insert import CheckInsert
from tables.check.update import CheckUpdate
from tables.check.get_all import CheckGetAll
from tables.check.get_by_login import CheckGetByLogin

ALL_CHECKS = (
    CheckInsert,
    CheckUpdate,
    CheckDelete,
    CheckGetAll,
    CheckGetByLogin,
)
ALL_NAMES = [x.name for x in ALL_CHECKS]
