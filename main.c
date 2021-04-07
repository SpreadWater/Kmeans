#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MaxTeamNumber 15//球队数量
#define MaxYearNumber 3//年份数量
#define MaxCenterClusterNumber 3//选取的中心簇数量
#define MaxNumber 1000.0//定义的最大成绩值
#define MinNumber 0.0//定义的最小成绩值

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
    CENTERCLUSTERID_TYPE centerClusterId;//所属中心簇的ID
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

void findCenterCluster(Team *CenterClusters, Team *teams);

void idToTeamName(int id);

void idToClusterName(int id);

int main() {
    Team teams[MaxTeamNumber];
    Team centerClusters[MaxCenterClusterNumber];

    init_GradeData(teams);
    standardGrade(teams);
    init_CenterCluster(centerClusters, teams);
    init_Instance(centerClusters, teams);
    findCenterCluster(centerClusters, teams);
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
        printf("请输入你所选择中心簇的ID");
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
            for (int j = 0; j < MaxYearNumber; ++j) {
                fscanf(fp, " %lf", &teams[i].years[j].grade);
            }
        }
    }
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

void findCenterCluster(Team *CenterClusters, Team *teams) {
    INSTANCE_TYPE min = MaxNumber;
    for (int i = 0; i < MaxTeamNumber; ++i) {
        for (int j = 0; j < MaxCenterClusterNumber; ++j) {
            if (min > teams[i].instances[j]) {
                min = teams[i].instances[j];
                teams[i].centerClusterId = CenterClusters[j].teamId;
            }
        }
        min=MaxNumber;
    }
}

void idToClusterName(int id) {
    switch (id) {
        case 1:
            printf("A类:\t");
            break;
        case 2:
            printf("B类:\t");
            break;
        case 3:
            printf("C类:\t");
            break;
    }
}

void idToTeamName(int id) {
    switch (id) {
        case 1:
            printf("1.中国\t");
            break;
        case 2:
            printf("2.日本\t");
            break;
        case 3:
            printf("3.韩国\t");
            break;
        case 4:
            printf("4.伊朗\t");
            break;
        case 5:
            printf("5.沙特\t");
            break;
        case 6:
            printf("6.伊拉克\t");
            break;
        case 7:
            printf("7.卡塔尔\t");
            break;
        case 8:
            printf("8.阿联酋\t");
            break;
        case 9:
            printf("9.乌兹别克斯坦\t");
            break;
        case 10:
            printf("10.泰国\t");
            break;
        case 11:
            printf("11.越南\t");
            break;
        case 12:
            printf("12.阿曼\t");
            break;
        case 13:
            printf("13.巴林\t");
            break;
        case 14:
            printf("14.朝鲜\t");
            break;
        case 15:
            printf("15.印尼\t");
            break;
    }
 }
