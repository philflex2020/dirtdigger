import configparser
from collections import OrderedDict
parser = configparser.ConfigParser()
parser.read_dict(
    OrderedDict((
        ('m1',
         OrderedDict((
             ('servo', 'servo1'),
             ('dest', '1000'),
             ('time', '100'),
             ))
         ),
        ('m2',
         OrderedDict((
             ('servo', 'servo2'),
             ('dest', '500'),
             ('time', '100'),
             ))
         ),
        ('m3',
         OrderedDict((
             ('servo', 'servo3'),
             ('dest', '600'),
             ('time', '100'),
             ))
         ),
        ))
    )
parser.sections()
with open('moved.ini', 'w') as configfile:
    parser.write(configfile)
        
