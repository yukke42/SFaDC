# (WIP) SFaDC

The implementation of ["Joint SFM and Detection Cues for Monocular 3D Localization in Road Scenes"](https://www.cv-foundation.org/openaccess/content_cvpr_2015/html/Song_Joint_SFM_and_2015_CVPR_paper.html) .



## requirements 

- Ubuntu 
- docker
- docker-compose

downlaod left color images, camera calibration matrices, and training labels of [KITTI tracking datasets](http://www.cvlibs.net/datasets/kitti/eval_tracking.php)  
Unzip and put them under `datasets` directory.




## Usage

### Build 

```
$ docker-compose build
```

### Run

```
$ xhost +local:docker
$ docker-compose run app bash
# cmkae .. && make -j8
# ./demo IMAGE_SET_ID
```