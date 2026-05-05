#pragma once

enum class JobOfferType {
    OIL_RAFFINER,
    AGRICULTOR
};

struct JobOffer {
    JobOfferType type;
    int salaryEstimation;
};
