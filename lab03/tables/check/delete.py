from time import time

from tables.check.base import CheckBase
from tables.models import User


class CheckDelete(CheckBase):
    name = 'delete'
    graph_title = 'Delete'

    def create_rows(self, rows):
        User.objects.all().delete()
        for i in range(rows):
            User.objects.create(id=i, login=f'testlogin{i}', password='testpas', role='testrole',
                                 moderator_id=None)

    def check_rows(self, rows):
        self.create_rows(rows)
        start_time = time()
        for i in range(rows):
            User.objects.filter(pk=i).delete()
        time_taken = time() - start_time
        return time_taken
