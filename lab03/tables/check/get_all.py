from time import time

from tables.check.base import CheckBase
from tables.models import User


class CheckGetAll(CheckBase):
    name = 'get_all'
    graph_title = 'get all'

    def check_rows(self, rows):
        User.objects.all().delete()
        for i in range(rows):
            User.objects.create(id=i, login=f'testlogin{i}', password='testpas', role='testrole',
                                 moderator_id=None)
        start_time = time()
        for i in range(rows):
            User.objects.all()
        time_taken = time() - start_time
        return time_taken
