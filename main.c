#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MaxTeamNumber 15//�������
#define MaxYearNumber 3//�������
#define MaxCenterClusterNumber 3//ѡȡ�����Ĵ�����
#define MaxNumber 1000.0//��������ɼ�ֵ
#define MinNumber 0.0//�������С�ɼ�ֵ

typedef double INSTANCE_TYPE;//��ӵ����Ĵصľ�����������
typedef double GRADE_TYPE;//�ض���ɼ�����������  0.1875���������0.1899995��float���µġ�������Ҫ�����double
typedef int TEAMID_TYPE;//��ӵ�ID��������
typedef int CENTERCLUSTERID_TYPE;//���Ĵص�ID��������
/*
 * ��ݽṹ��
 */
typedef struct year {
    GRADE_TYPE grade;
} Year;

/*
 * ���ҽṹ��
 */
typedef struct team {
    TEAMID_TYPE teamId;//��ӵ�ID
    CENTERCLUSTERID_TYPE centerClusterId;//�������Ĵص�ID
    Year years[MaxYearNumber];//�������
    INSTANCE_TYPE instances[MaxCenterClusterNumber];//��ӵ����Ĵصľ�������
} Team;

/*
 * ��Ҫ�õ���һЩ���߷���
 */
GRADE_TYPE findMax(GRADE_TYPE *grades);//�����ɼ�
GRADE_TYPE findMin(GRADE_TYPE *grades);//����С�ɼ�
/*
 * ����������飬���й�񻯴���
 */
void standardGrade(Team *teams);

/*
 * �������ֵ����Сֵ�ʹ���񻯵�ֵ
 */
GRADE_TYPE standardData(GRADE_TYPE Max, GRADE_TYPE Min, GRADE_TYPE unStandardData);

/*
 * ��ʼ�����Ĵأ����ݴ�������id���г�ʼ��
 */
void init_CenterCluster(Team *CenterClusters, Team *teams);

/*
 * ��ʼ���ɼ�����
 */
void init_GradeData(Team *teams);

/*
 * ��ʼ���������� 
 */
void init_Instance(Team *CenterClusters, Team *teams);

/*
 * �����������Ҷ��󣬽��о������
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
    //�������뱣����λ����
    result = (int) (100.0 * result + 0.5) / 100.0;
    return result;
}

void standardGrade(Team *teams) {
    GRADE_TYPE grades[MaxYearNumber][MaxTeamNumber];//�������ݳɼ���ά����

    for (int i = 0; i < MaxYearNumber; ++i) {
        for (int j = 0; j < MaxTeamNumber; ++j) {
            grades[i][j] = teams[j].years[i].grade;
        }
    }
    //��񻯲���
    for (int j = 0; j < MaxTeamNumber; ++j) {
        for (int i = 0; i < MaxYearNumber; ++i) {
            GRADE_TYPE MaxGrade = findMax(grades[i]);
            GRADE_TYPE MinGrad = findMin(grades[i]);
            //��һ�����²���
            teams[j].years[i].grade = standardData(MaxGrade, MinGrad, teams[j].years[i].grade);
        }
    }
}

void init_CenterCluster(Team *CenterClusters, Team *teams) {
    for (int i = 0; i < MaxCenterClusterNumber; ++i) {
        printf("����������ѡ�����Ĵص�ID");
        int id;
        scanf("%d", &id);
        CenterClusters[i] = teams[id - 1];
        //�����޸�һ�����Ĵص�Id�������ӦA,B,C��
        CenterClusters[i].teamId = i + 1;
    }
}

void init_GradeData(Team *teams) {
    FILE *fp;
    if ((fp = fopen("D://forthdata//data.txt", "r")) == NULL) {
        printf("��ʧ��");
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
    //������λС������
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
            printf("A��:\t");
            break;
        case 2:
            printf("B��:\t");
            break;
        case 3:
            printf("C��:\t");
            break;
    }
}

void idToTeamName(int id) {
    switch (id) {
        case 1:
            printf("1.�й�\t");
            break;
        case 2:
            printf("2.�ձ�\t");
            break;
        case 3:
            printf("3.����\t");
            break;
        case 4:
            printf("4.����\t");
            break;
        case 5:
            printf("5.ɳ��\t");
            break;
        case 6:
            printf("6.������\t");
            break;
        case 7:
            printf("7.������\t");
            break;
        case 8:
            printf("8.������\t");
            break;
        case 9:
            printf("9.���ȱ��˹̹\t");
            break;
        case 10:
            printf("10.̩��\t");
            break;
        case 11:
            printf("11.Խ��\t");
            break;
        case 12:
            printf("12.����\t");
            break;
        case 13:
            printf("13.����\t");
            break;
        case 14:
            printf("14.����\t");
            break;
        case 15:
            printf("15.ӡ��\t");
            break;
    }
 }
