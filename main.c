#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MaxTeamNumber 15//球队数量
#define MaxYearNumber 3//年份数量
#define MaxCenterClusterNumber 3//选取的中心簇数量
#define MaxNumber 1000.0//定义的最大成绩值
#define MinNumber 0.0//定义的最小成绩值
#define NullFlag 0//用0表示空

typedef double INSTANCE_TYPE;//球队到中心簇的距离数据类型
typedef double GRADE_TYPE;//重定义成绩的数据类型  0.1875四舍五入成0.1899995是float导致的。所以需要定义成double
typedef int TEAMID_TYPE;//球队的ID数据类型
typedef int CENTERCLUSTERID_TYPE;//中心簇的ID数据类型
/*
 * 年份结构体
 */
typedef struct year {
    GRADE_TYPE grade;
} Year;

/*
 * 国家结构体
 */
typedef struct team {
    TEAMID_TYPE teamId;//球队的ID
    CENTERCLUSTERID_TYPE centerClusterId;//所属中心簇的首次ID
    Year years[MaxYearNumber];//年份数组
    INSTANCE_TYPE instances[MaxCenterClusterNumber];//球队到中心簇的距离数组
} Team;

/*
 * 需要用到的一些工具方法
 */
GRADE_TYPE findMax(GRADE_TYPE *grades);//找最大成绩
GRADE_TYPE findMin(GRADE_TYPE *grades);//找最小成绩
/*
 * 传入球队数组，进行规格化处理
 */
void standardGrade(Team *teams);

/*
 * 传入最大值，最小值和待规格化的值
 */
GRADE_TYPE standardData(GRADE_TYPE Max, GRADE_TYPE Min, GRADE_TYPE unStandardData);

/*
 * 初始化中心簇，根据传进来的id进行初始化
 */
void init_CenterCluster(Team *CenterClusters, Team *teams);

/*
 * 初始化成绩数据
 */
void init_GradeData(Team *teams);

/*
 * 初始化距离数据 
 */
void init_Instance(Team *CenterClusters, Team *teams);

/*
 * 传入两个国家对象，进行距离计算
 */
GRADE_TYPE caculateInstance(Team team, Team centerCluster);

int findCenterCluster(Team *CenterClusters, Team *teams);

void caculateNewCenterCluster(Team *CenterClusters, Team *teams);

int returnArrayLength(TEAMID_TYPE *ids);

/*
 * 用于找到相同类的teamId
 */
void findSameCluster(Team *teams, TEAMID_TYPE *idArray, CENTERCLUSTERID_TYPE id);

/*
 * 初始化空的数组
 */
void newNullArray(TEAMID_TYPE *idArray);

/*
 * 聚类化操作，1.计算距离，2.分类中心簇 3.重新计算中心簇--->（判断一手是否更新中心簇）1--->2
 */
void k_means(Team *CenterClusters, Team *teams);

void printResult(Team *teams);

void idToTeamName(int id);

void idToClusterName(int id);

int main() {
    Team teams[MaxTeamNumber];
    Team centerClusters[MaxCenterClusterNumber];

    init_GradeData(teams);//初始化读取并规格化数据
    init_CenterCluster(centerClusters, teams);//初始化中心簇
    k_means(centerClusters,teams);//k-means算法
    return 0;
}

GRADE_TYPE findMax(GRADE_TYPE *grades) {
    GRADE_TYPE result = MinNumber;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (grades[i] > result) {
            result = grades[i];
        }
    }
    return result;
}

GRADE_TYPE findMin(GRADE_TYPE *grades) {
    GRADE_TYPE result = MaxNumber;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (grades[i] < result) {
            result = grades[i];
        }
    }
    return result;
}

GRADE_TYPE standardData(GRADE_TYPE Max, GRADE_TYPE Min, GRADE_TYPE unStandardData) {
    GRADE_TYPE result;
    result = (unStandardData - Min) / (Max - Min);
    //四舍五入保留两位操作
    result = (int) (100.0 * result + 0.5) / 100.0;
    return result;
}

void standardGrade(Team *teams) {
    GRADE_TYPE grades[MaxYearNumber][MaxTeamNumber];//纵向的年份成绩二维数组

    for (int i = 0; i < MaxYearNumber; ++i) {
        for (int j = 0; j < MaxTeamNumber; ++j) {
            grades[i][j] = teams[j].years[i].grade;
        }
    }
    //规格化操作
    for (int j = 0; j < MaxTeamNumber; ++j) {
        for (int i = 0; i < MaxYearNumber; ++i) {
            GRADE_TYPE MaxGrade = findMax(grades[i]);
            GRADE_TYPE MinGrad = findMin(grades[i]);
            //归一化更新操作
            teams[j].years[i].grade = standardData(MaxGrade, MinGrad, teams[j].years[i].grade);
        }
    }
}

void init_CenterCluster(Team *CenterClusters, Team *teams) {
    for (int i = 0; i < MaxCenterClusterNumber; ++i) {
        printf("请输入你所选择中心簇的ID\n");
        int id;
        scanf("%d", &id);
        CenterClusters[i] = teams[id - 1];
        //重新修改一下中心簇的Id，方便对应A,B,C类
        CenterClusters[i].teamId = i + 1;
    }
}

void init_GradeData(Team *teams) {
    FILE *fp;
    if ((fp = fopen("D://forthdata//data.txt", "r")) == NULL) {
        printf("打开失败");
        exit(0);
    } else {
        for (int i = 0; i < MaxTeamNumber; ++i) {
            teams[i].teamId = i + 1;
            teams[i].centerClusterId = NullFlag;//初始化中心簇为空

            for (int j = 0; j < MaxYearNumber; ++j) {
                fscanf(fp, " %lf", &teams[i].years[j].grade);
            }
        }
    }
    standardGrade(teams);//规格化处理
}

GRADE_TYPE caculateInstance(Team team, Team centerCluster) {
    GRADE_TYPE result;
    result = sqrt(pow(team.years[0].grade - centerCluster.years[0].grade, 2) +
                  pow(team.years[1].grade - centerCluster.years[1].grade, 2) +
                  pow(team.years[2].grade - centerCluster.years[2].grade, 2));
    //保留四位小数操作
    result = (int) (10000.0 * result + 0.5) / 10000.0;
    return result;
}

void init_Instance(Team *CenterClusters, Team *teams) {
    for (int i = 0; i < MaxTeamNumber; ++i) {
        for (int j = 0; j < MaxCenterClusterNumber; ++j) {
            teams[i].instances[j] = caculateInstance(teams[i], CenterClusters[j]);
        }
    }
}

int findCenterCluster(Team *CenterClusters, Team *teams) {
    INSTANCE_TYPE min = MaxNumber;
    int isUpdate = 0;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        CENTERCLUSTERID_TYPE clusterId = 0;
        for (int j = 0; j < MaxCenterClusterNumber; ++j) {
            if (min > teams[i].instances[j]) {
                min = teams[i].instances[j];
                clusterId = CenterClusters[j].teamId;
            }
        }
        //第一次设置
        if (teams[i].centerClusterId == NullFlag) {
            teams[i].centerClusterId = clusterId;
            isUpdate=1;
        } else if (teams[i].centerClusterId != clusterId) {
            //第二次设置
            teams[i].centerClusterId = clusterId;
            isUpdate = 1;//表示已经更新需要再次计算距离。
        }
        min = MaxNumber;
    }
    return isUpdate;
}

int returnArrayLength(TEAMID_TYPE *ids) {
    int result = 0;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (ids[i] != NullFlag) {
            result += 1;
        } else {
            break;
        }
    }
    return result;
}

void newNullArray(TEAMID_TYPE *idArray) {
    for (int i = 0; i < MaxTeamNumber; ++i) {
        idArray[i] = NullFlag;
    }
}

void findSameCluster(Team *teams, TEAMID_TYPE *idArray, CENTERCLUSTERID_TYPE id) {
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (teams[i].centerClusterId == id) {
            //轮训，按顺序插入idArray
            for (int j = 0; j < MaxTeamNumber; ++j) {
                if (idArray[j] == NullFlag) {
                    idArray[j] = teams[i].teamId;
                    break;
                }
            }
        }
    }
}

void caculateNewCenterCluster(Team *CenterClusters, Team *teams) {
    TEAMID_TYPE idArray[MaxTeamNumber];//记录同一类的球队的ID
    //初始化数组
    newNullArray(idArray);
    //计算新的中心点
    for (int k = 0; k < MaxCenterClusterNumber; ++k) {
        //首先先查找同类的teamid
        findSameCluster(teams, idArray, k + 1);

        int length = returnArrayLength(idArray);
        //计算新的距离
        for (int i = 0; i < MaxCenterClusterNumber; ++i) {
            INSTANCE_TYPE newInstance = 0;
            for (int j = 0; j < length; ++j) {
                int teamId = idArray[j];
                newInstance += teams[teamId - 1].years[i].grade;
            }
            newInstance /= length;
            CenterClusters[k].years[i].grade = newInstance;
        }
        newNullArray(idArray);
    }
}

void k_means(Team *CenterClusters, Team *teams) {
    int count = 0;
    int isUpdate = 1;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        if (isUpdate==1){
            init_Instance(CenterClusters, teams);//计算球队到中心簇的距离
            isUpdate=findCenterCluster(CenterClusters, teams);//分类到对应的中心簇
            caculateNewCenterCluster(CenterClusters, teams);//计算新的中心距离
            count++;
        }else{
            break;
        }
    }
    printf("k-means算法结束，迭代%d次\n",count);
    printResult(teams);
}
void printResult(Team *teams){
    for (int i = 0; i <MaxTeamNumber ; ++i) {
        idToTeamName(teams[i].teamId);
        idToClusterName(teams[i].centerClusterId);
    }
}
void idToClusterName(int id) {
    switch (id) {
        case 1:
            printf("A ");
            break;
        case 2:
            printf("B ");
            break;
        case 3:
            printf("C ");
            break;
    }
}

void idToTeamName(int id) {
    switch (id) {
        case 1:
            printf("1.中国");
            break;
        case 2:
            printf("2.日本");
            break;
        case 3:
            printf("3.韩国");
            break;
        case 4:
            printf("4.伊朗");
            break;
        case 5:
            printf("5.沙特");
            break;
        case 6:
            printf("6.伊拉克");
            break;
        case 7:
            printf("7.卡塔尔");
            break;
        case 8:
            printf("8.阿联酋");
            break;
        case 9:
            printf("9.乌兹别克斯坦");
            break;
        case 10:
            printf("10.泰国");
            break;
        case 11:
            printf("11.越南");
            break;
        case 12:
            printf("12.阿曼");
            break;
        case 13:
            printf("13.巴林");
            break;
        case 14:
            printf("14.朝鲜");
            break;
        case 15:
            printf("15.印尼");
            break;
    }
}
