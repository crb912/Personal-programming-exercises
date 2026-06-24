# super　解决了继承问题的方法调用顺序　MRO
import inspect


class Base(object):
    def __init__(self):
        print("Base init'ed")


# 继承base的__int__，但是没有使用super
class ChildA(Base):
    def __init__(self):
        print("ChildA init'ed")
        Base.__init__(self)


class ChildB(Base):
    def __init__(self):
        print("ChildB init'ed")
        super(ChildB, self).__init__()


class UserDependency(Base):
    def __init__(self):
        print("UserDependency init'ed")
        super(UserDependency, self).__init__()


class UserA(ChildA, UserDependency):
    def __init__(self):
        print("UserA init'ed")
        super(UserA, self).__init__()


class UserB(ChildB, UserDependency):
    def __init__(self):
        print("UserB init'ed")
        super(UserB, self).__init__()


# 这个结果发现: UserA没有调用UserDependency的方法__init__，特征是没有执行其构造方法的Print语句 MRO[UserA, ChildA, Base]
UserA()
print(UserA.__mro__)
print('\n')
# UserB正常调用了UserB的__int__方法，特征是[]
UserB()
print(UserB.__mro__)
