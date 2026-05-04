#pragma once

enum class JobOfferType {
    OIL_RAFFINER
};

struct JobOffer {
    JobOfferType type;
    int salaryEstimation;
};
