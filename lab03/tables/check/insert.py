from time import time

from tables.check.base import CheckBase
from tables.models import User


class CheckInsert(CheckBase):
    name = 'insert'
    graph_title = 'Insert'

    def check_rows(self, rows):
        User.objects.all().delete()
        start_time = time()
        for i in range(rows):
            User.objects.create(id=i, login=f'testlogin{i}', password='testpas', role='testrole',
                                 moderator_id=None)
        time_taken = time() - start_time
        return time_taken
