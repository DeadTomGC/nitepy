import ctypes
lib=ctypes.CDLL('nitepy')

track = lib.Tracker_new()
lib.getUserSkeletonHeadX.restype = float
lib.getUserSkeletonHeadY.restype = float
lib.getUserSkeletonHeadZ.restype = float

lib.getUserSkeletonNeckX.restype = float
lib.getUserSkeletonNeckY.restype = float
lib.getUserSkeletonNeckZ.restype = float

lib.getUserSkeletonL_ShX.restype = float
lib.getUserSkeletonL_ShY.restype = float
lib.getUserSkeletonL_ShZ.restype = float

lib.getUserSkeletonR_ShX.restype = float
lib.getUserSkeletonR_ShY.restype = float
lib.getUserSkeletonR_ShZ.restype = float

lib.getUserSkeletonL_ElbowX.restype = float
lib.getUserSkeletonL_ElbowY.restype = float
lib.getUserSkeletonL_ElbowZ.restype = float

lib.getUserSkeletonR_ElbowX.restype = float
lib.getUserSkeletonR_ElbowY.restype = float
lib.getUserSkeletonR_ElbowZ.restype = float

lib.getUserSkeletonL_HandX.restype = float
lib.getUserSkeletonL_HandY.restype = float
lib.getUserSkeletonL_HandZ.restype = float

lib.getUserSkeletonR_HandX.restype = float
lib.getUserSkeletonR_HandY.restype = float
lib.getUserSkeletonR_HandZ.restype = float

lib.getUserSkeletonTorsoX.restype = float
lib.getUserSkeletonTorsoY.restype = float
lib.getUserSkeletonTorsoZ.restype = float

lib.getUserSkeletonL_HipX.restype = float
lib.getUserSkeletonL_HipY.restype = float
lib.getUserSkeletonL_HipZ.restype = float

lib.getUserSkeletonR_HipX.restype = float
lib.getUserSkeletonR_HipY.restype = float
lib.getUserSkeletonR_HipZ.restype = float

lib.getUserSkeletonL_KneeX.restype = float
lib.getUserSkeletonL_KneeY.restype = float
lib.getUserSkeletonL_KneeZ.restype = float

lib.getUserSkeletonR_KneeX.restype = float
lib.getUserSkeletonR_KneeY.restype = float
lib.getUserSkeletonR_KneeZ.restype = float

lib.getUserSkeletonL_FootX.restype = float
lib.getUserSkeletonL_FootY.restype = float
lib.getUserSkeletonL_FootZ.restype = float

lib.getUserSkeletonR_FootX.restype = float
lib.getUserSkeletonR_FootY.restype = float
lib.getUserSkeletonR_FootZ.restype = float

while(True):
    lib.loop(track)
    if lib.getUsersCount(track) > 0:
        headX = lib.getUserSkeletonHeadX(track, 0)
        headY = lib.getUserSkeletonHeadY(track, 0)
        headZ = lib.getUserSkeletonHeadZ(track, 0)
        print str(headX) + " " + str(headY) + " " + str(headZ)
