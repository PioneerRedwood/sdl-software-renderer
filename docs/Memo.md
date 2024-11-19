# Memo

## 데카르트 좌표계; Cartesian coordinate system
[영문 위키](https://en.wikipedia.org/wiki/Cartesian_coordinate_system)

### 오일러 각; Euler Angles
[EulerAngles](https://mathworld.wolfram.com/EulerAngles.html)

## 3차원 공간 좌표를 스크린(2D) 공간으로 변환하여 렌더링

### [OpenGL Rendering Pipeline](http://www.songho.ca/opengl/gl_pipeline.html)
- OpenGL 파이프라인은 일종의 순서가 있는 처리 스테이지로 구성 
- 정점 기반 데이터와 픽셀 기반 데이터, 두 종류는 이 파이프라인에서 처리되고 조합하여 프레임 버퍼에 쓰여짐
- OpenGL은 처리된 데이터를 애플리케이션 쪽으로도 반대로 가져올 수 있음

Display List
- 후에 실행하기 위해 저장된(컴파일된) OpenGL 명령 그룹
- 모든 데이터, 기하 정보(정점) 그리고 픽셀 데이터는 여기에 저장될 수 있음
- 명령과 데이터 모두 캐시될 수 있기 때문에 성능 향상될 수 있음
- 가능하면 여기에 한번 수행되고 수정하지 않는 일종의 명령 그룹을 저장하는 것을 권장 
- 자세한 것은 문서 확인 [OpenGL Display List](http://www.songho.ca/opengl/gl_displaylist.html)

Vertex Operation
- 각 정점과 일반적인 좌표가 GL_MODELVIEW 매트릭스 (모델 좌표계 -> 카메라 좌표계로 변환)에 의해 전송됨
- 만약 라이팅이 활성화되어 있다면, 각 정점의 라이팅 계산은 전송된 정점과 일반 좌표을 사용함
- 자세한 것은 문서 확인 [Transformation](http://www.songho.ca/opengl/gl_transform.html)
    - 아래 다른 영역으로 별도 설명

Primitives Assembly
- Vertex Operation이 종료된 후 프리미티브들 (점, 선 그리고 폴리곤)은 다시 투영 행렬로 변환된 후 시각적으로 유효한 클리핑 평면 내부로 클리핑됨 (카메라 좌표계 -> 클립 좌표계)
- 그 후 3D 영역이 화면 공간 좌표계로 변환하기 위해 w 값에 의한 원근 분할과 뷰포트 변환이 적용됨
- 컬링이 활성화된 경우, 컬링 테스트 수행

Pixel Transfer Operation
- 클라이언트의 메모리를 읽은 뒤, 데이터는 스케일링, bias(?), 매핑 그리고 클램핑됨; 이러한 명령은 `Pixel Transfer Operation`이라 함
- 전송된 데이터는 텍스처 메모리에 저장되고 프래그먼트에 곧바로 래스터라이즈됨

Texture Memory
- 텍스처 이미지는 기하 객체에 적용되기 위해 텍스처 메모리에 로드됨

Rasterization
- 래스터라이제이션은 두 기하 정보와 픽셀 데이터가 프래그먼트로 변환되는 과정
- 프래그먼트는 색상, 깊이, 선 두께와 정점 크기와 `antialiasing` 계산 방식(GL_POINT_SMOOTH, GL_LINE_SMOOTH 그리고 GL_POLYGON_SMOOTH)이 담긴 사각 배열
- 만약 쉐이딩 모드가 GL_FILL인 경우, 폴리곤 영역 내부는 이때 채워짐
- 각 프래그먼트는 프레임버퍼의 각 픽셀과 대응됨

Fragment Operation
- 프래그먼트를 프레임버퍼로 변환하는 마지막 과정
- 첫번째로 텍셀 제네레이션; 텍스처 요소는 텍스처 메모리로부터 생성되고 각 프래그먼트에 적용
- 그 뒤 fog(?) 계산 적용
- 그 뒤 여러 개의 프래그먼트 테스트를 순서대로 수행; Scissor Test -> Alpha Test -> Stenicl Test -> Depth Test
- 마지막으로 블랜딩, 디더링, 논리 작업(logical operations)을 수행하고 비트마스크에 의한 마스킹을 수행하여 실제 픽셀 데이터가 프레임버퍼에 저장되도록 함

### [OpenGL Transformation](http://www.songho.ca/opengl/gl_transform.html)
3차원 공간 객체는 다음의 과정을 거쳐 2차원 공간으로 그려진다. 
Object Coordinates -> 

### [Transformations in OpenGL](https://nccastaff.bournemouth.ac.uk/jmacey/Lectures/OpenGL/transforms/?print-pdf#/) 
- OpenGL에서의 좌표 공간과 그 개념을 ppt로 설명 (영국 본머스 대학의 교재로 추정)

### [World, View and Projection Matrix Unveiled](https://web.archive.org/web/20131222170415/http:/robertokoci.com/world-view-projection-matrix-unveiled/) 
- DirectX 기준 설명
- 행렬, 단위 행렬 간단 설명
- 월드 혹은 모델 매트릭스 (World or Model Matrix)
    - 이동, 회전과 크기와 관련한 정보를 담고 있음
- 뷰 혹은 카메라 매트릭스 (View or Camera Matrix)
    - 카메라 좌표 공간으로 기준을 옮기는 행렬
    - 만약 원근법이 필요하지 않다면 이 변환까지 적용된 행렬로 화면에 표시 가능
- 프로젝션 매트릭스 (Projection Matrix; 투영 행렬)
    - 뷰로부터의 near, far 거리, 화면의 해상도와 카메라의 보는 각도 기반 행렬

정리
DirectX 기준: matrix4x4 World_View_Projection_Matrix = `World x View x Projection`
OpenGL 기준: matrix4x4 Projection_View_World_Matrix = `Projection x View x World`

### [Tutorial Series: Learning How to Write a 3D Soft Engine from Scratch in C#](https://www.davrous.com/2013/06/13/tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-typescript-or-javascript/)

### [OpenGL Transformation](http://www.songho.ca/opengl/gl_transform.html)


### [Breakdown of the LookAt function in OpenGL](https://arienbv.org/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/)

