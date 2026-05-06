import api from './api';
import type { ApiResponse } from '../types/api';

const reportingService = {
  getStats: async (): Promise<ApiResponse<any>> => {
    return api.get('/reports/stats');
  },

  getSubmissionTrends: async (): Promise<ApiResponse<any>> => {
    return api.get('/reports/submissions/trends');
  },

  getReviewerPerformance: async (): Promise<ApiResponse<any>> => {
    return api.get('/reports/reviewers/performance');
  }
};

export default reportingService;
