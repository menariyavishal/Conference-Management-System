import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { CheckCircle, Clock, Search, BookOpen, AlertCircle, Loader } from 'lucide-react';
import { useAuth } from '../../context/AuthContext';
import reviewService from '../../services/review.service';
import type { Review } from '../../types/api';

const ReviewerDashboard: React.FC = () => {
  const { user } = useAuth();
  const [reviews, setReviews] = useState<Review[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [stats, setStats] = useState({ assigned: 0, pending: 0, completed: 0 });
  const [searchTerm, setSearchTerm] = useState('');
  
  useEffect(() => {
    const fetchData = async () => {
      try {
        setIsLoading(true);
        const response = await reviewService.list();
        if (response.success) {
          const fetchedReviews = response.data;
          setReviews(fetchedReviews);
          
          // Calculate stats
          const newStats = {
            assigned: fetchedReviews.length,
            pending: fetchedReviews.filter(r => r.status === 'PENDING').length,
            completed: fetchedReviews.filter(r => r.status === 'COMPLETED').length
          };
          setStats(newStats);
        }
      } catch (error) {
        console.error("Error fetching reviews:", error);
      } finally {
        setIsLoading(false);
      }
    };

    fetchData();
  }, []);

  const filteredReviews = reviews.filter(r => 
    r.submissionId.toLowerCase().includes(searchTerm.toLowerCase()) || 
    r.comments?.toLowerCase().includes(searchTerm.toLowerCase())
  );

  const containerVariants: any = {
    hidden: { opacity: 0 },
    show: { opacity: 1, transition: { staggerChildren: 0.1 } }
  };
  
  const itemVariants: any = {
    hidden: { opacity: 0, y: 20 },
    show: { opacity: 1, y: 0, transition: { type: "spring", stiffness: 300, damping: 24 } }
  };

  return (
    <motion.div variants={containerVariants} initial="hidden" animate="show" className="space-y-8">
      {/* Banner */}
      <motion.div variants={itemVariants} className="relative rounded-3xl overflow-hidden shadow-2xl">
        <div className="absolute inset-0">
          <img 
            src="https://images.unsplash.com/photo-1456324504439-367cee3b3c32?auto=format&fit=crop&w=1200&q=80" 
            alt="Review Desk" 
            className="w-full h-full object-cover opacity-30 mix-blend-overlay"
          />
          <div className="absolute inset-0 bg-gradient-to-r from-indigo-900/90 to-[#0a0f1e]/90" />
        </div>
        <div className="relative px-8 py-12 sm:px-12 sm:py-16">
          <h1 className="text-3xl sm:text-4xl font-extrabold tracking-tight text-white mb-2">
            Reviewer Portal
          </h1>
          <p className="text-lg text-indigo-200 max-w-2xl font-medium">
            Welcome, {user?.fullName}. Thank you for contributing your expertise to our peer review process.
          </p>
          {stats.pending > 0 && (
            <div className="mt-8">
              <div className="inline-flex items-center px-4 py-2 rounded-lg bg-indigo-500/20 border border-indigo-500/30 text-indigo-300">
                <AlertCircle className="w-5 h-5 mr-2" />
                <span className="text-sm font-semibold">You have {stats.pending} reviews pending completion.</span>
              </div>
            </div>
          )}
        </div>
      </motion.div>

      {/* Stats */}
      <div className="grid grid-cols-1 gap-6 sm:grid-cols-3">
        {[
          { label: 'Assigned Papers', value: stats.assigned, icon: BookOpen, color: 'text-indigo-400', bg: 'bg-indigo-400/10' },
          { label: 'Pending Reviews', value: stats.pending, icon: Clock, color: 'text-amber-400', bg: 'bg-amber-400/10' },
          { label: 'Completed', value: stats.completed, icon: CheckCircle, color: 'text-emerald-400', bg: 'bg-emerald-400/10' }
        ].map((stat, idx) => (
          <motion.div key={idx} variants={itemVariants} whileHover={{ y: -5 }} className="bg-[#0f172a]/80 backdrop-blur-xl border border-white/5 rounded-2xl p-6 shadow-xl transition-all">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-sm font-medium text-slate-400 uppercase tracking-wider">{stat.label}</p>
                <p className="mt-2 text-4xl font-extrabold text-white">{stat.value}</p>
              </div>
              <div className={`p-4 rounded-xl ${stat.bg}`}>
                <stat.icon className={`w-8 h-8 ${stat.color}`} />
              </div>
            </div>
          </motion.div>
        ))}
      </div>

      {/* Assignments list */}
      <motion.div variants={itemVariants} className="bg-[#0f172a]/80 backdrop-blur-xl border border-white/5 rounded-3xl shadow-xl overflow-hidden">
        <div className="px-6 py-5 border-b border-white/10 flex flex-col sm:flex-row justify-between items-center gap-4">
          <h3 className="text-xl font-bold text-white">Current Assignments</h3>
          <div className="relative">
            <Search className="w-5 h-5 text-slate-400 absolute left-3 top-1/2 -translate-y-1/2" />
            <input 
              type="text" 
              placeholder="Search assignments..." 
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="bg-white/5 border border-white/10 rounded-xl pl-10 pr-4 py-2 text-white text-sm focus:border-indigo-500 focus:ring-1 focus:ring-indigo-500 w-full sm:w-64"
            />
          </div>
        </div>

        {isLoading ? (
          <div className="p-20 flex flex-col items-center justify-center">
            <Loader className="w-10 h-10 text-indigo-500 animate-spin mb-4" />
            <p className="text-slate-400">Loading assignments...</p>
          </div>
        ) : filteredReviews.length > 0 ? (
          <div className="overflow-x-auto">
            <table className="w-full text-left border-collapse">
              <thead>
                <tr className="bg-white/5">
                  <th className="px-6 py-4 text-sm font-semibold text-slate-300">Submission ID</th>
                  <th className="px-6 py-4 text-sm font-semibold text-slate-300">Status</th>
                  <th className="px-6 py-4 text-sm font-semibold text-slate-300 text-right">Actions</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-white/5">
                {filteredReviews.map((review) => (
                  <tr key={review.id || review.reviewId} className="hover:bg-white/5 transition-colors">
                    <td className="px-6 py-4">
                      <p className="text-sm font-bold text-white">{review.submissionId}</p>
                    </td>
                    <td className="px-6 py-4">
                      <span className={`inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-medium border ${
                        review.status === 'COMPLETED' 
                          ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/20' 
                          : 'bg-amber-500/10 text-amber-400 border-amber-500/20'
                      }`}>
                        {review.status}
                      </span>
                    </td>
                    <td className="px-6 py-4 text-right">
                      <button className="text-indigo-400 hover:text-indigo-300 font-bold text-sm transition-colors">
                        {review.status === 'COMPLETED' ? 'View Review' : 'Start Review'}
                      </button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        ) : (
          <div className="p-8 text-center">
            <div className="inline-flex items-center justify-center w-16 h-16 rounded-full bg-slate-800 mb-4">
              <CheckCircle className="w-8 h-8 text-slate-500" />
            </div>
            <h3 className="text-lg font-medium text-white mb-1">All caught up!</h3>
            <p className="text-slate-400">There are no pending reviews assigned to you at the moment.</p>
          </div>
        )}
      </motion.div>
    </motion.div>
  );
};

export default ReviewerDashboard;
